import re
from pathlib import Path

def parse_battle_report(path: str = "./src/logs/log_world.txt") -> None:
    fire_counts   = []
    damage_counts = []
    total_fire_turns = None
    hit_count_sum    = 0
    in_hit_section   = False

    with Path(path).open(encoding="utf-8") as fp:
        for raw in fp:
            line = raw.strip()

            # 각 유닛의 Fire / Damage 추출
            if m := re.search(r"Fire\s*Count\s*=\s*(\d+)", line):
                fire_counts.append(int(m.group(1)))
            if m := re.search(r"Damage\s*Count\s*=\s*(\d+)", line):
                damage_counts.append(int(m.group(1)))

            # Total Fire Turns 추출
            if m := re.search(r"Total\s*Fire\s*Turns\s*[:=]\s*(\d+)", line):
                total_fire_turns = int(m.group(1))

            # Hit Count 표 시작·종료 판별
            if line.startswith("===== Hit Count"):
                in_hit_section = True
                continue
            if in_hit_section:
                # 빈 줄·구분선 만나면 끝
                if not line or line.startswith("====="):
                    in_hit_section = False
                    continue
                # 표 내부 숫자 합산
                hit_count_sum += sum(map(int, line.split()))

    # 총합 계산
    fire_count_sum   = sum(fire_counts)
    damage_count_sum = sum(damage_counts)

    # 유효탄 비율
    eff_fire_ratio = (damage_count_sum / fire_count_sum) if fire_count_sum else 0

    # --- 결과 보고 ---
    print("=== Battle Report Summary ===")
    print(f"Total Fire Count (Event Generation) : {fire_count_sum}")
    print(f"Total Fire Turns (Engine Run): {total_fire_turns}")
    print(f"Total Hit Count  (Events reached an AM) : {hit_count_sum}")
    print(f"Total Damage Count (Valid Hits) : {damage_count_sum}")
    print()
    print(f"Effective Fire Ratio (Damage / Fire) : {eff_fire_ratio:.2%}")
    print(f"Expected Ratio When 5 x 5 map, 3 enemy = 3/25 = {3.0/25.0*100}%")

if __name__ == "__main__":
    parse_battle_report("./src/logs/log_world.txt")
