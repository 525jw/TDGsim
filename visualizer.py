#!/usr/bin/env python3
"""
Timeline visualizer for TDGsim run logs.

It reuses the map styling from map_reader.py and replays the events recorded
in logs/log_world.txt. Each simulation time-step updates unit positions,
draws small rectangles for both sides, and briefly overlays shot lines when
fire events occur. Killed units disappear from the board.
"""

from __future__ import annotations

import argparse
import json
import os
import re
from collections import defaultdict
from dataclasses import dataclass, field
from typing import Dict, Iterable, List, Optional, Tuple

import pygame

# --- Rendering colours (same palette as map_reader.py) ---------------------
COLORS = {
    "bg": (247, 247, 247),
    "grid": (200, 200, 200),
    "water": (158, 202, 225),
    "bridge": (205, 170, 102),
    "forest": (161, 217, 155),
    "urban": (189, 189, 189),
    "rough": (231, 186, 82),
    "plain": (220, 220, 220),
    "blue": (49, 130, 189),
    "blue_e": (8, 81, 156),
    "red": (222, 45, 38),
    "red_e": (165, 15, 21),
    "white": (255, 255, 255),
    "shot": (255, 215, 0),
}


def glyph(unit_type: str) -> str:
    unit_type = (unit_type or "").lower()
    if unit_type.startswith(("rifle", "inf")):
        return "R"
    if unit_type.startswith(("tank", "arm")):
        return "T"
    return (unit_type[:3] or "?").upper()


def infer_side(unit_name: str) -> str:
    upper = unit_name.upper()
    if upper.startswith("BLUE"):
        return "BLUE"
    if upper.startswith("RED"):
        return "RED"
    return "NEUTRAL"


def map_patch_colour(kind: str) -> Tuple[int, int, int]:
    return COLORS.get(kind.lower(), COLORS["plain"])


# --- Data containers -------------------------------------------------------
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
        if unit in self.positions:
            del self.positions[unit]
        self.dead_units.add(unit)

    def ensure_info(self, unit: str) -> UnitInfo:
        if unit not in self.unit_info:
            self.unit_info[unit] = UnitInfo(side=infer_side(unit), unit_type="")
        return self.unit_info[unit]

    def lookup(self, unit: str) -> Optional[Tuple[int, int]]:
        return self.positions.get(unit) or self.last_known.get(unit)

    def reset(self) -> None:
        self.positions = dict(self.initial_positions)
        self.last_known = dict(self.initial_positions)
        self.dead_units.clear()


# --- Log parsing -----------------------------------------------------------
MOVE_LINE = re.compile(
    r"\[(?P<unit>[^\]]+)\]\s+\|\s+Task:\s+MOVE\s+\|\s+From:\s+\("
    r"(?P<fx>-?\d+),\s*(?P<fy>-?\d+)\)\s+\|\s+To:\s+\("
    r"(?P<tx>-?\d+),\s*(?P<ty>-?\d+)\)"
)
TIME_PATTERN = re.compile(r"when Time\s*:\s*([0-9]+(?:\.[0-9]+)?)")


def parse_log(log_path: str) -> Tuple[Dict[float, List[dict]], Dict[str, Tuple[int, int]]]:
    events: Dict[float, List[dict]] = defaultdict(list)
    pending_move: Dict[str, Tuple[int, int]] = {}
    inferred_initials: Dict[str, Tuple[int, int]] = {}

    current_time = 0.0

    with open(log_path, "r", encoding="utf-8") as handle:
        for raw_line in handle:
            line = raw_line.strip()
            if not line:
                continue

            time_match = TIME_PATTERN.search(line)
            if time_match:
                current_time = float(time_match.group(1))

            move_match = MOVE_LINE.match(line)
            if move_match:
                unit = move_match.group("unit").strip()
                fx, fy = int(move_match.group("fx")), int(move_match.group("fy"))
                tx, ty = int(move_match.group("tx")), int(move_match.group("ty"))
                pending_move[unit] = (tx, ty)
                inferred_initials.setdefault(unit, (fx, fy))
                continue

            if "moved to ordered position" in line:
                unit = line.split("]", 1)[0][1:].strip()
                dest = pending_move.get(unit)
                if dest:
                    events[current_time].append(
                        {"type": "move", "unit": unit, "pos": dest}
                    )
                continue

            if "] shoot " in line:
                unit = line.split("]", 1)[0][1:].strip()
                remainder = line.split("]", 1)[1].strip()
                body = remainder.split("when Time", 1)[0].strip()

                # Handle the special "shoot at missing target" diagnostic
                if body.startswith("shoot at missing target"):
                    events[current_time].append(
                        {"type": "shoot", "unit": unit, "target": None}
                    )
                    continue

                if body.startswith("shoot "):
                    target = body[len("shoot ") :].strip()
                    if target:
                        events[current_time].append(
                            {"type": "shoot", "unit": unit, "target": target}
                        )
                continue

            if " is dead" in line:
                unit = line.split("]", 1)[0][1:].strip()
                events[current_time].append({"type": "death", "unit": unit})
                continue

    return events, inferred_initials


# --- Map loading -----------------------------------------------------------
def load_map(map_path: str) -> Tuple[dict, Dict[str, Tuple[int, int]], Dict[str, UnitInfo]]:
    with open(map_path, "r", encoding="utf-8") as handle:
        spec = json.load(handle)

    units_pos: Dict[str, Tuple[int, int]] = {}
    info: Dict[str, UnitInfo] = {}

    for unit in spec.get("units", []):
        uid = unit["uid"]
        units_pos[uid] = (int(unit["x"]), int(unit["y"]))
        info[uid] = UnitInfo(side=unit.get("side", "NEUTRAL").upper(), unit_type=unit.get("type", ""))

    return spec, units_pos, info


# --- Rendering helpers -----------------------------------------------------
def draw_background(screen: pygame.Surface, width: int, height: int, cell: int, patches: Iterable[dict]) -> None:
    screen.fill(COLORS["bg"])

    for x in range(width + 1):
        pygame.draw.line(
            screen, COLORS["grid"], (x * cell, 0), (x * cell, height * cell), 1
        )
    for y in range(height + 1):
        pygame.draw.line(
            screen, COLORS["grid"], (0, y * cell), (width * cell, y * cell), 1
        )

    for patch in patches:
        colour = map_patch_colour(patch.get("kind", "plain"))
        x1, y1, x2, y2 = patch["x1"], patch["y1"], patch["x2"], patch["y2"]
        rect = pygame.Rect(
            min(x1, x2) * cell,
            min(y1, y2) * cell,
            (abs(x2 - x1) + 1) * cell,
            (abs(y2 - y1) + 1) * cell,
        )
        pygame.draw.rect(screen, colour, rect)


def draw_units(
    screen: pygame.Surface,
    state: SimulationState,
    cell: int,
    font: pygame.font.Font,
) -> None:
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
        screen.blit(
            label,
            (x * cell + (cell - label.get_width()) // 2, y * cell + (cell - label.get_height()) // 2),
        )


def draw_shots(screen: pygame.Surface, shots: Iterable[ShotOverlay], cell: int) -> None:
    for shot in shots:
        sx, sy = shot.source
        tx, ty = shot.target
        start = (sx * cell + cell // 2, sy * cell + cell // 2)
        end = (tx * cell + cell // 2, ty * cell + cell // 2)
        pygame.draw.line(screen, shot.colour, start, end, 3)


# --- Playback --------------------------------------------------------------
def build_timeline(events: Dict[float, List[dict]]) -> List[Tuple[float, List[dict]]]:
    return sorted((time, evts) for time, evts in events.items())


def describe_event(ev: dict) -> str:
    etype = ev["type"]
    unit = ev.get("unit", "?")
    if etype == "move":
        pos = ev["pos"]
        return f"{unit} moved to {pos}"
    if etype == "shoot":
        target = ev.get("target") or "unknown"
        return f"{unit} shot {target}"
    if etype == "death":
        return f"{unit} eliminated"
    return repr(ev)


def playback(
    spec: dict,
    state: SimulationState,
    timeline: List[Tuple[float, List[dict]]],
    *,
    cell_size: int,
    fps: int,
    step_delay_ms: int,
) -> None:
    width, height = spec["w"], spec["h"]
    patches = spec.get("patches", [])

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

    running = True
    step_once = False
    while running:
        dt = clock.tick(fps)
        accumulator += dt

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN:
                if event.key in (pygame.K_ESCAPE, pygame.K_q):
                    running = False
                elif event.key == pygame.K_SPACE:
                    paused = not paused
                elif event.key in (pygame.K_RIGHT, pygame.K_RETURN):
                    paused = True
                    step_once = True
                elif event.key == pygame.K_r:
                    # Reset timeline
                    state.reset()
                    current_index = 0
                    current_time = 0.0
                    current_shots = []
                    last_event_text = "Reset"
                    paused = True
                    accumulator = 0.0
                    step_once = False

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
                step_once = False
                accumulator = 0.0

            step_time, events = timeline[current_index]
            current_shots = []
            texts = []

            for ev in events:
                etype = ev["type"]
                if etype == "move":
                    state.set_position(ev["unit"], ev["pos"])
                elif etype == "shoot":
                    shooter = ev["unit"]
                    target = ev.get("target")
                    shooter_pos = state.lookup(shooter)
                    target_pos = state.lookup(target) if target else None
                    info = state.ensure_info(shooter)
                    colour = COLORS["blue_e"] if info.side == "BLUE" else COLORS["red_e"]
                    if shooter_pos and target_pos:
                        current_shots.append(
                            ShotOverlay(source=shooter_pos, target=target_pos, colour=colour)
                        )
                elif etype == "death":
                    state.remove_unit(ev["unit"])
                texts.append(describe_event(ev))

            current_time = step_time
            last_event_text = "; ".join(texts) if texts else f"Time {step_time}: no change"
            current_index += 1

        draw_background(screen, width, height, cell_size, patches)
        draw_units(screen, state, cell_size, font)
        draw_shots(screen, current_shots, cell_size)

        hud_lines = [
            f"Time: {current_time:.2f}",
            f"{'Paused' if paused else 'Running'}",
            last_event_text,
            "SPACE=Pause  RIGHT=Step  ESC=Quit",
        ]
        for i, line in enumerate(hud_lines):
            surf = hud_font.render(line, True, COLORS["blue_e"])
            screen.blit(surf, (8, 8 + i * (hud_font.get_height() + 2)))

        pygame.display.flip()

    pygame.quit()


# --- Command line ----------------------------------------------------------
def main() -> None:
    parser = argparse.ArgumentParser(description="Replay TDGsim logs on the tactical map.")
    parser.add_argument("--map", default="map.json", help="Path to map specification JSON.")
    parser.add_argument(
        "--log",
        default=os.path.join("logs", "log_world.txt"),
        help="Simulation log file to replay.",
    )
    parser.add_argument("--cell", type=int, default=10, help="Pixel size of each map cell.")
    parser.add_argument("--fps", type=int, default=60, help="Render frames per second.")
    parser.add_argument(
        "--interval",
        type=int,
        default=600,
        help="Milliseconds to wait before advancing to the next simulation time-step.",
    )

    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    map_path = args.map if os.path.isabs(args.map) else os.path.join(script_dir, args.map)
    log_path = args.log if os.path.isabs(args.log) else os.path.join(script_dir, args.log)

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
    )


if __name__ == "__main__":
    main()
