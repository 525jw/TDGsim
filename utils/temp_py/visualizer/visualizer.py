#!/usr/bin/env python3
"""
Timeline visualizer for TDGsim run logs.

- Loads scenario.json (or legacy map.json) for initial unit layout
- Parses run logs into (time -> events)
- Replays:
    * MOVE : 기존 위치에서 새 위치로 표시(확정 MOVE 라인 즉시 반영)
    * FIRE : 사수 -> 목표로 선(좌표면 좌표, 유닛명이면 그 유닛 위치, 없으면 사수 자기좌표)
    * DEAD : 팀과 무관하게 보드에서 제거

Run:
    python visualizer.py
"""

from __future__ import annotations

import argparse
import json
import math
import os
import re
from collections import defaultdict
from dataclasses import dataclass, field
from typing import Dict, Iterable, List, Optional, Set, Tuple

import pygame

# ---------------------- Colors / Glyphs ----------------------
COLORS = {
    "bg": (247, 247, 247),
    "grid": (200, 200, 200),
    "water": (158, 202, 225),
    "bridge": (205, 170, 102),
    "forest": (161, 217, 155),
    "urban": (189, 189, 189),
    "rough": (231, 186, 82),
    "plain": (220, 220, 220),
    "target_area": (180, 180, 180),
    "blue": (49, 130, 189),
    "blue_e": (8, 81, 156),
    "red": (222, 45, 38),
    "red_e": (165, 15, 21),
    "white": (255, 255, 255),
    "objective": (0, 0, 0),
}

OBJECTIVE_TARGET_UNIT = "BLUE-PLT1"

def glyph(unit_type: str) -> str:
    t = (unit_type or "").lower()
    if t.startswith(("rifle", "inf")): return "R"
    if t.startswith(("tank", "arm")):  return "T"
    return (t[:3] or "?").upper()

def infer_side(unit_name: str) -> str:
    u = (unit_name or "").upper()
    if u.startswith("BLUE"): return "BLUE"
    if u.startswith("RED"):  return "RED"
    return "NEUTRAL"

def map_patch_colour(kind: str) -> Tuple[int, int, int]:
    return COLORS.get((kind or "").lower(), COLORS["plain"])

# ---------------------- Name normalization ----------------------
_SUFFIXES = {"MNV", "FIR", "FIRE", "DET", "MNE"}  # 확실한 접미만 제거

def base_unit_name(name: str) -> str:
    """ 'BLUE-PLT1-SOL13-MNV' -> 'BLUE-PLT1-SOL13' """
    if not name: return name
    s = name.strip().strip("[]")
    parts = s.split("-")
    if len(parts) >= 2:
        last = parts[-1]
        if last.isalpha() and len(last) <= 4 and last.upper() in _SUFFIXES:
            return "-".join(parts[:-1])
    return s

# ---------------------- Data Types ----------------------
@dataclass
class UnitInfo:
    side: str = "NEUTRAL"
    unit_type: str = ""

@dataclass
class ShotOverlay:
    source: Tuple[int, int]
    target: Tuple[int, int]
    colour: Tuple[int, int, int]

@dataclass
class SimulationState:
    unit_info: Dict[str, UnitInfo]
    positions: Dict[str, Tuple[int, int]]
    initial_positions: Dict[str, Tuple[int, int]] = field(default_factory=dict)
    last_known: Dict[str, Tuple[int, int]] = field(default_factory=dict)
    dead_units: set = field(default_factory=set)

    def __post_init__(self) -> None:
        if not self.initial_positions:
            self.initial_positions = dict(self.positions)
        if not self.last_known:
            self.last_known = dict(self.positions)
        for name, pos in self.positions.items():
            self.last_known[name] = pos

    def set_position(self, unit: str, pos: Tuple[int, int]) -> None:
        self.positions[unit] = pos
        self.last_known[unit] = pos
        self.dead_units.discard(unit)

    def remove_unit(self, unit: str) -> None:
        self.positions.pop(unit, None)
        self.dead_units.add(unit)

    def ensure_info(self, unit: str) -> UnitInfo:
        if unit not in self.unit_info:
            self.unit_info[unit] = UnitInfo(side=infer_side(unit), unit_type="")
        return self.unit_info[unit]

    def lookup(self, unit: Optional[str]) -> Optional[Tuple[int, int]]:
        if not unit: return None
        return self.positions.get(unit) or self.last_known.get(unit)

    def reset(self) -> None:
        self.positions = dict(self.initial_positions)
        self.last_known = dict(self.initial_positions)
        self.dead_units.clear()

# ---------------------- Log Parsing ----------------------
# 시간: "[1.000]" 또는 "when Time : 1.000"
TIME_RE_A = re.compile(r"when Time\s*:\s*([0-9]+(?:\.[0-9]+)?)")
TIME_RE_B = re.compile(r"\[(?:t\s*=\s*)?([0-9]+(?:\.[0-9]+)?)\]")

# 공통: 앞에 타임스탬프가 올 수 있음
TS_PREFIX = r"(?:\[\s*[0-9]+(?:\.[0-9]+)?\s*\]\s*)?"

# MOVE A: "[UNIT] | Task: MOVE | From: (...) | To: (...)"
MOVE_RE_A = re.compile(
    r"\[(?P<unit>[^\]]+)\]\s*\|\s*Task:\s*MOVE\b.*?\bFrom:\s*\(\s*(?P<fx>-?\d+)\s*,\s*(?P<fy>-?\d+)\s*\)"
    r".*?\bTo:\s*\(\s*(?P<tx>-?\d+)\s*,\s*(?P<ty>-?\d+)\s*\)"
)

# MOVE B: "[t] UNIT : MOVE from=(x, y) to=(x, y)"  ← 네 로그 포맷
MOVE_RE_B = re.compile(
    TS_PREFIX +
    r"(?P<unit>[A-Za-z0-9\-_.]+)\s*[:|]\s*MOVE\b.*?\bfrom\s*=\s*\(\s*(?P<fx>-?\d+)\s*,\s*(?P<fy>-?\d+)\s*\)"
    r".*?\bto\s*=\s*\(\s*(?P<tx>-?\d+)\s*,\s*(?P<ty>-?\d+)\s*\)",
    re.IGNORECASE,
)

# RECEIVE_ORDER MOVE: 초기좌표 추정만
RECV_MOVE_RE = re.compile(
    TS_PREFIX +
    r"(?P<unit>[A-Za-z0-9\-_.]+)\s*[:|]\s*RECEIVE_ORDER\b.*?\btask\s*=\s*MOVE\b.*?\bfrom\s*=\s*\(\s*(?P<fx>-?\d+)\s*,\s*(?P<fy>-?\d+)\s*\)"
    r".*?\bto\s*=\s*\(\s*(?P<tx>-?\d+)\s*,\s*(?P<ty>-?\d+)\s*\)",
    re.IGNORECASE,
)

# FIRE/SHOOT 라인: "[t] UNIT : FIRE shoot at TARGET" 등
SHOOT_LINE_RE = re.compile(
    TS_PREFIX + r"(?P<unit>[A-Za-z0-9\-_.]+)\s*[:|].*\b(FIRE|SHOOT)\b(?P<body>.*)$",
    re.IGNORECASE,
)
COORD_IN_PARENS_RE = re.compile(r"\(\s*(?P<x>-?\d+)\s*,\s*(?P<y>-?\d+)\s*\)")
UNIT_NAME_RE = re.compile(r"\b([A-Za-z0-9]+-[A-Za-z0-9\-_.]+)\b")
MISSING_TARGET_RE = re.compile(r"shoot\s+at\s+missing\s+target", re.IGNORECASE)

# DEAD: "[t] UNIT : DEAD" 또는 "[t] UNIT is dead"
DEAD_RE_A = re.compile(TS_PREFIX + r"(?P<unit>[A-Za-z0-9\-_.]+)\s*is\s*dead\b", re.IGNORECASE)
DEAD_RE_B = re.compile(TS_PREFIX + r"(?P<unit>[A-Za-z0-9\-_.]+)\s*[:|].*\bDEAD\b", re.IGNORECASE)

def parse_log(log_path: str) -> Tuple[Dict[float, List[dict]], Dict[str, Tuple[int, int]]]:
    events: Dict[float, List[dict]] = defaultdict(list)
    inferred_initials: Dict[str, Tuple[int, int]] = {}

    current_time = 0.0

    with open(log_path, "r", encoding="utf-8", errors="ignore") as fh:
        for raw in fh:
            line = raw.strip()
            if not line:
                continue

            # 시간 앵커
            tm = TIME_RE_A.search(line) or TIME_RE_B.search(line)
            if tm:
                try:
                    current_time = float(tm.group(1))
                except Exception:
                    pass  # 못 읽어도 진행

            # --- MOVE 확정: A/B 포맷 -> 바로 이벤트 추가 ---
            mm = MOVE_RE_A.search(line) or MOVE_RE_B.search(line)
            if mm:
                unit_raw = mm.group("unit").strip()
                unit = base_unit_name(unit_raw)
                fx, fy = int(mm.group("fx")), int(mm.group("fy"))
                tx, ty = int(mm.group("tx")), int(mm.group("ty"))
                inferred_initials.setdefault(unit, (fx, fy))
                events[current_time].append({"type": "move", "unit": unit, "pos": (tx, ty)})
                continue

            # RECEIVE_ORDER MOVE: 초기좌표 추정(이벤트는 아님)
            rm = RECV_MOVE_RE.search(line)
            if rm:
                unit_raw = rm.group("unit").strip()
                unit = base_unit_name(unit_raw)
                fx, fy = int(rm.group("fx")), int(rm.group("fy"))
                inferred_initials.setdefault(unit, (fx, fy))
                continue

            # FIRE/SHOOT
            sm = SHOOT_LINE_RE.search(line)
            if sm:
                unit = base_unit_name(sm.group("unit").strip())
                body = sm.group("body") or ""

                if MISSING_TARGET_RE.search(body):
                    events[current_time].append({"type": "shoot", "unit": unit, "target": None})
                    continue

                coords = list(COORD_IN_PARENS_RE.finditer(body))
                if coords:
                    for cm in coords:
                        tx, ty = int(cm.group("x")), int(cm.group("y"))
                        events[current_time].append(
                            {"type": "shoot", "unit": unit, "target_coord": (tx, ty)}
                        )
                    continue

                name_candidates = []
                for raw in UNIT_NAME_RE.findall(body):
                    base = base_unit_name(raw)
                    if base and base not in name_candidates:
                        name_candidates.append(base)
                if name_candidates:
                    for target in name_candidates:
                        events[current_time].append({"type": "shoot", "unit": unit, "target": target})
                    continue

                lowered = body.lower()
                for token in ("shoot at", "shoot", "fire at", "fire"):
                    if token in lowered:
                        seg = lowered.split(token, 1)[1].strip()
                        raw_target = seg.split()[0] if seg else ""
                        target = base_unit_name(raw_target) if raw_target else None
                        events[current_time].append({"type": "shoot", "unit": unit, "target": target})
                        break
                else:
                    events[current_time].append({"type": "shoot", "unit": unit, "target": None})
                continue

            # DEAD
            dm = DEAD_RE_A.search(line) or DEAD_RE_B.search(line)
            if dm:
                unit = base_unit_name(dm.group("unit"))
                events[current_time].append({"type": "death", "unit": unit})
                continue

    return events, inferred_initials

# ---------------------- Map Loading ----------------------
def load_scenario_map(spec: dict) -> Tuple[dict, Dict[str, Tuple[int, int]], Dict[str, UnitInfo]]:
    size = spec.get("size") or {}
    width = int(size.get("w", spec.get("w", 0)) or 0)
    height = int(size.get("h", spec.get("h", 0)) or 0)
    width = max(width, 0)
    height = max(height, 0)

    def clamp(x: int, y: int) -> Tuple[int, int]:
        if width > 0:
            x = max(0, min(width - 1, x))
        if height > 0:
            y = max(0, min(height - 1, y))
        return x, y

    patches: List[dict] = []
    for area in spec.get("terrain", []):
        if not isinstance(area, dict):
            continue
        patches.append({
            "kind": area.get("kind", "plain"),
            "x1": int(area.get("x1", 0)),
            "y1": int(area.get("y1", 0)),
            "x2": int(area.get("x2", area.get("x1", 0))),
            "y2": int(area.get("y2", area.get("y1", 0))),
        })

    target_areas: List[dict] = []
    for area in spec.get("goal", []):
        if not isinstance(area, dict):
            continue
        target_areas.append({
            "x1": int(area.get("x1", 0)),
            "y1": int(area.get("y1", 0)),
            "x2": int(area.get("x2", area.get("x1", 0))),
            "y2": int(area.get("y2", area.get("y1", 0))),
        })

    units: List[dict] = []
    for entity in spec.get("entity", []):
        if not isinstance(entity, dict):
            continue
        name = entity.get("id") or entity.get("uid") or entity.get("name")
        if not name:
            continue
        count = max(1, int(entity.get("count", 1)))
        anchor = entity.get("anchor") if isinstance(entity.get("anchor"), dict) else {}
        ax = anchor.get("x", entity.get("x", 0))
        ay = anchor.get("y", entity.get("y", 0))
        try:
            ax = int(ax)
            ay = int(ay)
        except (TypeError, ValueError):
            ax, ay = 0, 0
        side = str(entity.get("side", "NEUTRAL")).upper()
        unit_type = entity.get("type", "")

        if count <= 1:
            x, y = clamp(ax, ay)
            units.append({"uid": name, "side": side, "type": unit_type, "x": x, "y": y})
        else:
            cols = max(1, int(math.ceil(math.sqrt(count))))
            for idx in range(count):
                row = idx // cols
                col = idx % cols
                x, y = clamp(ax + col, ay + row)
                uid = f"{name}-SOL{idx + 1:02d}"
                units.append({"uid": uid, "side": side, "type": unit_type, "x": x, "y": y})

    render_spec = {
        "w": width,
        "h": height,
        "patches": patches,
        "target_areas": target_areas,
        "units": units,
    }

    units_pos: Dict[str, Tuple[int, int]] = {}
    info: Dict[str, UnitInfo] = {}
    for u in units:
        uid = u["uid"]
        units_pos[uid] = (int(u["x"]), int(u["y"]))
        info[uid] = UnitInfo(side=str(u.get("side", "NEUTRAL")).upper(), unit_type=u.get("type", ""))

    return render_spec, units_pos, info


def load_map(map_path: str) -> Tuple[dict, Dict[str, Tuple[int, int]], Dict[str, UnitInfo]]:
    with open(map_path, "r", encoding="utf-8") as fh:
        spec = json.load(fh)

    if "size" in spec and "entity" in spec:
        return load_scenario_map(spec)

    units_pos: Dict[str, Tuple[int, int]] = {}
    info: Dict[str, UnitInfo] = {}

    for u in spec.get("units", []):
        uid = u.get("uid") or u.get("name") or f"{u.get('side','U')}-{u.get('type','U')}-{u.get('x')},{u.get('y')}"
        x, y = int(u["x"]), int(u["y"])
        units_pos[uid] = (x, y)
        info[uid] = UnitInfo(side=str(u.get("side","NEUTRAL")).upper(), unit_type=u.get("type",""))

    return spec, units_pos, info

def _add_candidate_point(target: Set[Tuple[int, int]], candidate) -> None:
    """Normalize assorted JSON coordinate formats into integer grid cells."""
    if candidate is None:
        return
    if isinstance(candidate, dict):
        if "x" in candidate and "y" in candidate:
            _add_candidate_point(target, (candidate["x"], candidate["y"]))
            return
        for key in ("cell", "point", "pos", "position", "coord"):
            if key in candidate:
                _add_candidate_point(target, candidate[key])
        for key in ("cells", "points", "positions", "coords", "tiles", "route"):
            if key in candidate:
                _add_candidate_point(target, candidate[key])
        return
    if isinstance(candidate, (list, tuple)):
        if len(candidate) == 2:
            try:
                x = int(float(candidate[0]))
                y = int(float(candidate[1]))
            except (ValueError, TypeError):
                return
            target.add((x, y))
        else:
            for item in candidate:
                _add_candidate_point(target, item)

def collect_objective_cells(spec: dict, unit_name: str) -> Set[Tuple[int, int]]:
    """Extract every destination cell declared for the given unit in map.json."""
    cells: Set[Tuple[int, int]] = set()

    objectives = spec.get("objectives")
    if isinstance(objectives, dict):
        if unit_name in objectives:
            _add_candidate_point(cells, objectives[unit_name])
        for value in objectives.values():
            if isinstance(value, dict) and base_unit_name(value.get("unit") or "") == unit_name:
                for key in ("cells", "points", "positions", "coords", "tiles", "route"):
                    if key in value:
                        _add_candidate_point(cells, value[key])
                if "x" in value and "y" in value:
                    _add_candidate_point(cells, (value["x"], value["y"]))
    elif isinstance(objectives, list):
        for entry in objectives:
            if isinstance(entry, dict):
                candidate_name = entry.get("unit") or entry.get("uid") or entry.get("name") or ""
                if base_unit_name(candidate_name) != unit_name:
                    continue
                for key in ("cells", "points", "positions", "coords", "tiles", "route"):
                    if key in entry:
                        _add_candidate_point(cells, entry[key])
                if "x" in entry and "y" in entry:
                    _add_candidate_point(cells, (entry["x"], entry["y"]))
            else:
                _add_candidate_point(cells, entry)

    for unit_spec in spec.get("units", []):
        if base_unit_name(unit_spec.get("uid") or unit_spec.get("name") or "") != unit_name:
            continue
        for key in ("objectives", "objective", "destinations", "destination", "route", "routes"):
            if key in unit_spec:
                _add_candidate_point(cells, unit_spec[key])

    return cells

# ---------------------- Rendering ----------------------
def draw_background(
    screen: pygame.Surface,
    width: int,
    height: int,
    cell: int,
    patches: Iterable[dict],
    target_areas: Optional[Iterable[dict]] = None,
    objective_cells: Optional[Iterable[Tuple[int, int]]] = None,
) -> None:
    screen.fill(COLORS["bg"])
    for x in range(width + 1):
        pygame.draw.line(screen, COLORS["grid"], (x * cell, 0), (x * cell, height * cell), 1)
    for y in range(height + 1):
        pygame.draw.line(screen, COLORS["grid"], (0, y * cell), (width * cell, y * cell), 1)

    for p in patches:
        colour = map_patch_colour(p.get("kind", "plain"))
        x1, y1, x2, y2 = p["x1"], p["y1"], p["x2"], p["y2"]
        rect = pygame.Rect(
            min(x1, x2) * cell, min(y1, y2) * cell,
            (abs(x2 - x1) + 1) * cell, (abs(y2 - y1) + 1) * cell
        )
        pygame.draw.rect(screen, colour, rect)

    if target_areas:
        for area in target_areas:
            try:
                x1, y1, x2, y2 = area["x1"], area["y1"], area["x2"], area["y2"]
            except (KeyError, TypeError):
                continue
            rect = pygame.Rect(
                min(x1, x2) * cell, min(y1, y2) * cell,
                (abs(x2 - x1) + 1) * cell, (abs(y2 - y1) + 1) * cell
            )
            pygame.draw.rect(screen, COLORS["target_area"], rect)

    if objective_cells:
        for ox, oy in objective_cells:
            rect = pygame.Rect(ox * cell, oy * cell, cell, cell)
            pygame.draw.rect(screen, COLORS["objective"], rect)

def draw_units(screen: pygame.Surface, state: SimulationState, cell: int, font: pygame.font.Font) -> None:
    for unit, pos in state.positions.items():
        info = state.unit_info.get(unit) or UnitInfo(side=infer_side(unit))
        x, y = pos
        box = pygame.Rect(x * cell, y * cell, cell, cell)

        if info.side == "BLUE":
            inner, edge = COLORS["blue"], COLORS["blue_e"]
        elif info.side == "RED":
            inner, edge = COLORS["red"], COLORS["red_e"]
        else:
            inner, edge = COLORS["urban"], COLORS["grid"]

        pygame.draw.rect(screen, inner, box)
        pygame.draw.rect(screen, edge, box, 2)

        tag = glyph(info.unit_type)
        label = font.render(tag, True, COLORS["white"])
        screen.blit(label, (x * cell + (cell - label.get_width()) // 2,
                            y * cell + (cell - label.get_height()) // 2))

def draw_shots(screen: pygame.Surface, shots: Iterable[ShotOverlay], cell: int) -> None:
    for shot in shots:
        sx, sy = shot.source
        tx, ty = shot.target
        start = (sx * cell + cell // 2, sy * cell + cell // 2)
        end = (tx * cell + cell // 2, ty * cell + cell // 2)
        pygame.draw.line(screen, shot.colour, start, end, 3)

# ---------------------- Playback ----------------------
def build_timeline(events: Dict[float, List[dict]]) -> List[Tuple[float, List[dict]]]:
    return sorted(events.items(), key=lambda kv: kv[0])

def describe_event(ev: dict) -> str:
    k = ev["type"]; u = ev.get("unit","?")
    if k == "move":  return f"{u} -> {ev['pos']}"
    if k == "shoot": return f"{u} fired"
    if k == "death": return f"{u} KIA"
    return repr(ev)

def playback(
    spec: dict,
    state: SimulationState,
    timeline: List[Tuple[float, List[dict]]],
    *,
    cell_size: int,
    fps: int,
    step_delay_ms: int,
    snapshot_count: int,
    snapshot_dir: Optional[str],
) -> None:
    width, height = spec["w"], spec["h"]
    patches = spec.get("patches", [])
    target_areas = spec.get("target_areas") or spec.get("target areas") or spec.get("targets") or spec.get("goal") or []
    objective_cells = collect_objective_cells(spec, OBJECTIVE_TARGET_UNIT)

    pygame.init()
    pygame.display.set_caption("TDGsim Timeline Visualizer")
    screen = pygame.display.set_mode((width * cell_size, height * cell_size))
    font = pygame.font.SysFont(None, max(14, cell_size // 2))
    hud_font = pygame.font.SysFont(None, 22)
    clock = pygame.time.Clock()

    current_index = 0
    current_time = 0.0
    current_shots: List[ShotOverlay] = []
    last_event_text = "Ready"
    paused = False
    accumulator = 0.0

    SHOT_HOLD_MS = 350
    shot_expire_at = 0

    running = True
    step_once = False

    snapshot_indices: Set[int] = set()
    if snapshot_count > 0 and timeline:
        count = min(snapshot_count, len(timeline))
        if count == 1:
            snapshot_indices = {len(timeline) - 1}
        else:
            for i in range(count):
                idx = int(round(i * (len(timeline) - 1) / (count - 1)))
                snapshot_indices.add(idx)
    if snapshot_indices and snapshot_dir:
        os.makedirs(snapshot_dir, exist_ok=True)
        for name in os.listdir(snapshot_dir):
            path = os.path.join(snapshot_dir, name)
            if os.path.isfile(path):
                os.remove(path)
    while running:
        dt = clock.tick(fps)
        accumulator += dt

        for ev in pygame.event.get():
            if ev.type == pygame.QUIT: running = False
            elif ev.type == pygame.KEYDOWN:
                if ev.key in (pygame.K_ESCAPE, pygame.K_q): running = False
                elif ev.key == pygame.K_SPACE: paused = not paused
                elif ev.key in (pygame.K_RIGHT, pygame.K_RETURN):
                    paused = True; step_once = True
                elif ev.key == pygame.K_r:
                    state.reset()
                    current_index = 0; current_time = 0.0
                    current_shots = []; last_event_text = "Reset"
                    paused = True; accumulator = 0.0; step_once = False

        if current_index >= len(timeline):
            paused = True

        should_step = False
        if not paused and current_index < len(timeline) and accumulator >= step_delay_ms:
            should_step = True
        elif step_once and current_index < len(timeline):
            should_step = True

        if should_step:
            if not paused:
                accumulator %= step_delay_ms
            else:
                step_once = False; accumulator = 0.0

            step_time, evts = timeline[current_index]
            current_shots = []
            shot_expire_at = pygame.time.get_ticks() + SHOT_HOLD_MS

            texts = []
            for e in evts:
                t = e["type"]
                if t == "move":
                    state.set_position(e["unit"], e["pos"])
                elif t == "shoot":
                    shooter = e["unit"]
                    shooter_pos = state.lookup(shooter)
                    target_pos = e.get("target_coord")
                    if target_pos is None:
                        tgt_name = e.get("target")
                        target_pos = state.lookup(tgt_name)
                    if target_pos is None:
                        target_pos = shooter_pos
                    if shooter_pos and target_pos:
                        side = state.ensure_info(shooter).side
                        if side == "BLUE":
                            colour = COLORS["blue"]
                        elif side == "RED":
                            colour = COLORS["red"]
                        else:
                            colour = COLORS["urban"]
                        current_shots.append(
                            ShotOverlay(source=shooter_pos, target=target_pos, colour=colour)
                        )
                elif t == "death":
                    state.remove_unit(e["unit"])
                texts.append(describe_event(e))

            current_time = step_time
            last_event_text = "; ".join(texts) if texts else f"{step_time:.2f}: (no change)"
            current_index += 1

        if current_shots and pygame.time.get_ticks() > shot_expire_at:
            current_shots = []

        draw_background(screen, width, height, cell_size, patches, target_areas, objective_cells)
        draw_units(screen, state, cell_size, font)
        draw_shots(screen, current_shots, cell_size)

        hud_lines = [
            f"Time: {current_time:.2f}",
            f"{'Paused' if paused else 'Running'}  step={current_index}/{len(timeline)}",
            last_event_text,
            "SPACE=Pause  RIGHT/ENTER=Step  R=Reset  ESC=Quit",
        ]
        for i, line in enumerate(hud_lines):
            surf = hud_font.render(line, True, COLORS["blue_e"])
            screen.blit(surf, (8, 8 + i * (hud_font.get_height() + 2)))

        pygame.display.flip()

        if snapshot_indices and (current_index - 1) in snapshot_indices and snapshot_dir:
            file_time = f"{current_time:.2f}".replace(".", "p")
            snap_name = f"snapshot_{current_index:02d}_t{file_time}.png"
            pygame.image.save(screen, os.path.join(snapshot_dir, snap_name))
            snapshot_indices.discard(current_index - 1)

    pygame.quit()

# ---------------------- Helpers ----------------------
def auto_find_log(script_dir: str) -> Optional[str]:
    log_dir = os.path.join(script_dir, "logs")
    candidates: List[str] = []
    if os.path.isdir(log_dir):
        for name in os.listdir(log_dir):
            lower = name.lower()
            if not lower.startswith("log_simulation"):
                continue
            if not lower.endswith(".txt"):
                continue
            p = os.path.join(log_dir, name)
            if os.path.isfile(p) and os.path.getsize(p) > 0:
                candidates.append(p)
    if candidates:
        return max(candidates, key=lambda p: (os.path.getmtime(p), p))

    legacy = [
        os.path.join(script_dir, "log_simulation.txt"),
        os.path.join(script_dir, "logs", "log_simulation.txt"),
        os.path.join(script_dir, "log_world.txt"),
        os.path.join(script_dir, "logs", "log_world.txt"),
    ]
    for p in legacy:
        if os.path.isfile(p) and os.path.getsize(p) > 0:
            return p
    return None


def find_repo_root(start_dir: str) -> str:
    cur = os.path.abspath(start_dir)
    while True:
        if os.path.isdir(os.path.join(cur, ".git")):
            return cur
        parent = os.path.dirname(cur)
        if parent == cur:
            return start_dir
        cur = parent


def resolve_log_path(arg: str, script_dir: str) -> Optional[str]:
    if not arg:
        return None
    candidates: List[str] = []
    if os.path.isabs(arg):
        candidates.append(arg)
    else:
        if arg.isdigit():
            candidates.append(os.path.join(script_dir, "logs", f"log_simulation_exp{int(arg):03d}.txt"))
        candidates.append(os.path.join(script_dir, arg))
        candidates.append(os.path.join(script_dir, "logs", arg))
        if not arg.lower().endswith(".txt"):
            candidates.append(os.path.join(script_dir, "logs", f"{arg}.txt"))
    for p in candidates:
        if os.path.isfile(p):
            return p
    return None

# ---------------------- CLI ----------------------
def main() -> None:
    parser = argparse.ArgumentParser(description="Replay TDGsim logs on the tactical map.")
    parser.add_argument("--map", help="Path to scenario/map specification JSON.")
    parser.add_argument("--log", help="Simulation log file to replay (path or exp index).")
    parser.add_argument("--cell", type=int, default=10, help="Pixel size of each map cell.")
    parser.add_argument("--fps", type=int, default=60, help="Render frames per second.")
    parser.add_argument("--interval", type=int, default=600, help="Milliseconds per simulation step.")
    parser.add_argument("--snapshots", type=int, default=0, help="Number of timeline snapshots to save (0 = off).")
    parser.add_argument("--snap-dir", default="snapshots", help="Directory to save snapshots.")
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    repo_root = find_repo_root(script_dir)

    map_path = args.map or os.path.join(repo_root, "data", "scenario.json")
    if not os.path.isabs(map_path):
        map_path = os.path.join(repo_root, map_path)

    if args.log:
        log_path = resolve_log_path(args.log, repo_root)
        if not log_path:
            auto = auto_find_log(repo_root)
            if not auto:
                raise FileNotFoundError(f"Log not found: {args.log}")
            log_path = auto
    else:
        log_path = auto_find_log(repo_root)
        if not log_path:
            raise FileNotFoundError("No log file found. Put one of these next to the script:\n"
                                    "  logs/log_simulation_exp###.txt, logs/log_simulation.txt, "
                                    "  log_world.txt")

    spec, positions, info = load_map(map_path)
    events, inferred_initials = parse_log(log_path)

    for unit, pos in inferred_initials.items():
        positions.setdefault(unit, pos)
        info.setdefault(unit, UnitInfo(side=infer_side(unit), unit_type=""))

    initial_positions = dict(positions)
    state = SimulationState(
        unit_info=info,
        positions=dict(initial_positions),
        initial_positions=initial_positions,
    )
    timeline = build_timeline(events)

    if not timeline:
        print("No events detected in log; nothing to replay.")
        return

    playback(
        spec,
        state,
        timeline,
        cell_size=args.cell,
        fps=args.fps,
        step_delay_ms=max(50, args.interval),
        snapshot_count=max(0, args.snapshots),
        snapshot_dir=os.path.join(script_dir, args.snap_dir) if args.snapshots > 0 else None,
    )

if __name__ == "__main__":
    main()
