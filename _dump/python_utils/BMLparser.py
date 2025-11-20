# BMLparser.py
from __future__ import annotations
from typing import Any, Dict, List, Optional
import copy


# 상수, 기본값, 필요할 때 바꿔야함
MOVE = "MOVE"
BOMBARD = "BOMBARD"
HOLD = "HOLD"
SUPPORTED_TASKS = {MOVE, BOMBARD, HOLD}

DEFAULT_TASK_PARAMS = {
    MOVE:    {"speed_mode": "NORMAL"},
    BOMBARD: {"ammo": "HE", "method": "TIME", "volume": {"duration": 180, "rate": "SUSTAINED"}},
    HOLD:    {"duration": 180},
}


DEFAULT_DUR = { MOVE: 180, BOMBARD: 180, HOLD: 180 }

class BMLParseError(ValueError):
    pass

def deepmerge(base: Dict[str, Any], over: Optional[Dict[str, Any]]) -> Dict[str, Any]:
    """
    base 딕셔너리에 over 딕셔너리를 재귀적으로 병합.
    - base를 복사하고,
    - over의 key/value를 하나씩 
        - 값이 딕셔너리이고 base에도 같은 key가 있으면 재귀적으로 병합
        - 아니면 덮어쓰기(복사)
    용도: 
    """
    out = copy.deepcopy(base)
    if not over:
        return out
    for k, v in over.items():
        if isinstance(v, dict) and isinstance(out.get(k), dict):
            out[k] = deepmerge(out[k], v)
        else:
            out[k] = copy.deepcopy(v)
    return out

def _as_Point(x: Any) -> List[float]:
    """
    좌표 형식이 유효한지 검사해서 [x,y] 부동소수점 리스트로 반환.
    int를 달라고 하면 int로 바꿔도 됨.
    """
    if isinstance(x, (list, tuple)) and len(x) == 2 and all(isinstance(v, (int, float)) for v in x):
        return [float(x[0]), float(x[1])]
    raise BMLParseError(f"좌표 형식 오류: {x} (예: [10,12])")

# 진영 추론
def infer_side(unit: str) -> str:
    u = unit.upper()
    if u.startswith("BLUE-"): return "BLUE"
    if u.startswith("RED-"):  return "RED"
    return "NEUTRAL"

# 병과/역할 추론
def infer_forcetype(unit: str) -> str:
    u = unit.upper()
    if "ART" in u: return "ART"
    if "TNK" in u or "TANK" in u: return "TNK"
    if "APC" in u: return "APC"
    if "MG"  in u: return "MG"
    if "HQ"  in u: return "HQ"
    return "INF"

# 편제 추론
def infer_echelon(unit: str) -> Optional[str]:
    u = unit.upper()
    if "-SQD" in u: return "SQD"
    if "-PLT" in u: return "PLT"
    if "-COY" in u: return "COY"
    if "-BN"  in u: return "BN"
    if "-BDE" in u: return "BDE"
    return None


# 파서 메인
def BMLparse(
    doc: Dict[str, Any],
    *,
    defaults_params: Dict[str, Any] = DEFAULT_TASK_PARAMS,
    defaults_dur: Dict[str, int] = DEFAULT_DUR,
    fallback_dur: int = 180,
    default_priority: int = 50,
    include_when: bool = True,          # False면 when_sim 자체를 생략
    include_default_dur: bool = True,   # False면 duration 입력 없으면 dur 생략
    passthrough_when_keys: List[str] = ("gate","pre","until","deadline","cancel_on","repeat","jitter"),
) -> Dict[str, Any]:
    """
    입력 doc의 'orders' 배열을 순회하면서 각 오더를 표준화된 오브젝트로 만듦.

    - WHO: unit/side/forcetype/echelon 추출(간단 추론 로직)
    - WHAT: task 및 기본 task_params 병합
    - WHERE: MOVE는 'to' 또는 'Point'를 where.Point로, route 작성
            BOMBARD는 what.task_params.Point로 표적 저장
    - WHEN_sim: duration 처리 및 선택적인 gate/pre 등 pass-through
    - 출력 스키마: {"version":..., "orders":[{who,what,where,when_sim?,priority,constraints}...]}
    

    MOVE/BOMBARD만 정규화. t0는 생성하지 않음.
    - MOVE: to|Point -> where.Point, route=[[x,y]]
    - BOMBARD: Point -> what.task_params.Point
    - 시간: when_sim에 dur만 옵션으로 전달(또는 완전 생략)
    """

    # 기본 유효성 검사
    if "orders" not in doc or not isinstance(doc["orders"], list):
        raise BMLParseError("'orders' 배열이 필요합니다.")

    out = {"version": "bml-v1-simtime", "orders": []}

    # 각 오더 처리
    for idx, od in enumerate(doc["orders"], start=1):
        unit = od.get("unit"); task = od.get("task")
        # 필수 필드 타입을 검사
        if not isinstance(unit, str) or not isinstance(task, str):
            raise BMLParseError(f"[order#{idx}] 'unit'과 'task'는 문자열이어야 합니다.")
        task = task.upper()
        if task not in SUPPORTED_TASKS:
            raise BMLParseError(f"[order#{idx}] 지원하지 않는 task: {task} (허용: {SUPPORTED_TASKS})")

        # WHO
        who = {"unit": unit, "side": infer_side(unit), "forcetype": infer_forcetype(unit)}
        ech = infer_echelon(unit)
        if ech: who["echelon"] = ech

        # WHAT
        base_params = defaults_params.get(task, {})
        task_params = deepmerge(base_params, {})

        # WHERE & FIRE 표적 처리
        where: Dict[str, Any] = {}
        if task == MOVE:
            if "to" in od:
                p = _as_Point(od["to"])
            elif "Point" in od:
                p = _as_Point(od["Point"])
            else:
                raise BMLParseError(f"[order#{idx}] MOVE에는 'to' 또는 'Point'가 필요합니다.")
            where["Point"] = p
            where["route"] = [p]
        elif task == BOMBARD:
            # 얘는 Point 필요
            if "Point" not in od:
                raise BMLParseError(f"[order#{idx}] BOMBARD에는 'Point'가 필요합니다.")
            task_params["Point"] = _as_Point(od["Point"])
        elif task == HOLD:
            # HOLD는 where 없음
            pass
        else: 
            # RECON 등 기타 task
            pass

        what = {"task": task, "task_params": task_params}

        # WHEN_sim (t0 없음)
        when_sim: Optional[Dict[str, Any]] = None
        if include_when:
            when_sim = {}
            raw_dur = od.get("duration")
            if raw_dur is None:
                if include_default_dur:
                    when_sim["dur"] = defaults_dur.get(task, fallback_dur)
                # else: dur 생략(모델이 자체 관리)
            elif isinstance(raw_dur, (int, float)):
                when_sim["dur"] = int(raw_dur)
            else:
                raise BMLParseError(f"[order#{idx}] duration 형식 오류: {raw_dur}")

            # 입력에 있으면 gate/pre/until 등은 그대로 pass-through
            for k in passthrough_when_keys:
                if k in od:
                    when_sim[k] = copy.deepcopy(od[k])

            # 빈 딕셔너리면 None으로(완전 생략)
            if not when_sim:
                when_sim = None
        # when_sim 처리 끝

        std = {
            "who": who,
            "what": what,
            "where": where,
            "priority": default_priority,
            "constraints": {}
        }
        if when_sim is not None:
            std["when_sim"] = when_sim  # dur/게이트만, t0 없음

        # 최소 의미검사 MOVE, BOMBARD에 필요한 정보 검증
        if task == MOVE and not (("route" in where and where["route"]) or ("Point" in where)):
            raise BMLParseError(f"[order#{idx}] MOVE: 공간 목표 필요.")
        if task == BOMBARD and not (task_params.get("Point") or where.get("Point")):
            raise BMLParseError(f"[order#{idx}] BOMBARD: 표적 필요.")

        out["orders"].append(std)

    return out

# 사용 예시
if __name__ == "__main__":
    import json, argparse
    EXAMPLE_BML = {
        "orders": [
            {"unit":"BLUE-PLT1","task":"MOVE","to":[10,12]},
            {"unit":"BLUE-PLT2","task":"MOVE","to":[9,12]},
            {"unit":"BLUE-PLT3","task":"MOVE","to":[8,11]},
            {"unit":"BLUE-MG","task":"MOVE","to":[7,13]},
            {"unit":"BLUE-TNK","task":"MOVE","to":[6,11]},
            {"unit":"BLUE-ART","task":"BOMBARD","Point":[22,5],"duration":3},
            {"unit":"RED-PLT1","task":"MOVE","to":[9,10]},
            {"unit":"RED-APC","task":"MOVE","to":[20,6]},
            {"unit":"RED-MG","task":"MOVE","to":[19,5]},
            {"unit":"RED-TNK","task":"MOVE","to":[10,7]},
            {"unit":"RED-ART","task":"BOMBARD","Point":[8,12],"duration":40},
        ]
    }

    p = argparse.ArgumentParser()
    p.add_argument("file", help="BML JSON file")
    p.add_argument("--out-file", "-o", help="output normalized json file (default: stdout)")

    args = p.parse_args()

    with open(args.file, "r", encoding="utf-8") as f:
        doc = json.load(f)

    normalized = BMLparse(doc, include_when=True, include_default_dur=True)

    out_json = json.dumps(normalized, ensure_ascii=False, indent=2)
    if args.out_file:
        with open(args.out_file, "w", encoding="utf-8") as fo:
            fo.write(out_json)
    else :
        print(out_json)

    '''
    # 1) dur은 기본값/입력값만, t0 없음
    normalized = BMLparse(EXAMPLE_BML, include_when=True, include_default_dur=True)
    print("=== with when_sim(dur only) ===")
    print(json.dumps(normalized, ensure_ascii=False, indent=2))

    # 2) when_sim 자체를 완전히 생략(모델이 전부 관리)
    normalized2 = BMLparse(EXAMPLE_BML, include_when=False)
    print("\n=== without when_sim ===")
    print(json.dumps(normalized2, ensure_ascii=False, indent=2))
    '''