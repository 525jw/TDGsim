#!/usr/bin/env python3
"""
Scenario editor for TDGsim scenario.json.

Controls:
  - Left click: select entity (click a unit in its footprint)
  - Drag: move selected entity anchor
  - Arrow keys: nudge anchor (Shift = 5 cells)
  - [ / ]: decrease/increase count
  - Tab: cycle entities
  - E/T: toggle entity/terrain edit mode
  - G: toggle goal edit mode
  - Terrain mode:
      * Drag empty space: create terrain rectangle
      * Drag existing terrain: move rectangle
      * Right click: delete terrain
      * 1-6: set terrain kind (plain/water/bridge/forest/urban/rough)
  - Goal mode:
      * Drag empty space: create goal rectangle
      * Drag existing goal: move rectangle
      * Right click: delete goal
  - S: save scenario.json
  - R: reload scenario.json
  - Q or ESC: quit
"""

from __future__ import annotations

import argparse
import json
import math
import os
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple

import pygame

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
    "selected": (255, 215, 0),
}

TERRAIN_KINDS = ["plain", "water", "bridge", "forest", "urban", "rough"]


def map_patch_colour(kind: str) -> Tuple[int, int, int]:
    return COLORS.get((kind or "").lower(), COLORS["plain"])


def find_repo_root(start_dir: str) -> str:
    cur = os.path.abspath(start_dir)
    while True:
        if os.path.isdir(os.path.join(cur, ".git")):
            return cur
        parent = os.path.dirname(cur)
        if parent == cur:
            return start_dir
        cur = parent


def clamp_point(x: int, y: int, width: int, height: int) -> Tuple[int, int]:
    if width > 0:
        x = max(0, min(width - 1, x))
    if height > 0:
        y = max(0, min(height - 1, y))
    return x, y


@dataclass
class UnitSlot:
    entity_index: int
    unit_index: int
    row: int
    col: int
    x: int
    y: int


@dataclass
class EntitySpec:
    index: int
    raw: dict
    name: str
    side: str
    kind: str
    count: int
    anchor: Tuple[int, int]
    units: List[UnitSlot] = field(default_factory=list)

    def rebuild(self, width: int, height: int) -> None:
        self.units.clear()
        count = max(1, int(self.count))
        cols = max(1, int(math.ceil(math.sqrt(count))))
        ax, ay = self.anchor
        for idx in range(count):
            row = idx // cols
            col = idx % cols
            x, y = clamp_point(ax + col, ay + row, width, height)
            self.units.append(UnitSlot(self.index, idx, row, col, x, y))


def load_scenario(path: str) -> Tuple[dict, int, int, List[dict], List[dict], List[EntitySpec]]:
    with open(path, "r", encoding="utf-8") as fh:
        spec = json.load(fh)

    size = spec.get("size") or {}
    width = int(size.get("w", spec.get("w", 0)) or 0)
    height = int(size.get("h", spec.get("h", 0)) or 0)

    terrain = [t for t in spec.get("terrain", []) if isinstance(t, dict)]
    goals = [g for g in spec.get("goal", []) if isinstance(g, dict)]

    entities: List[EntitySpec] = []
    for idx, ent in enumerate(spec.get("entity", [])):
        if not isinstance(ent, dict):
            continue
        name = ent.get("id") or ent.get("uid") or ent.get("name") or f"entity_{idx}"
        count = max(1, int(ent.get("count", 1)))
        anchor = ent.get("anchor") if isinstance(ent.get("anchor"), dict) else {}
        ax = anchor.get("x", ent.get("x", 0))
        ay = anchor.get("y", ent.get("y", 0))
        try:
            ax = int(ax)
            ay = int(ay)
        except (TypeError, ValueError):
            ax, ay = 0, 0
        ax, ay = clamp_point(ax, ay, width, height)
        entity = EntitySpec(
            index=idx,
            raw=ent,
            name=str(name),
            side=str(ent.get("side", "NEUTRAL")).upper(),
            kind=str(ent.get("type", "")),
            count=count,
            anchor=(ax, ay),
        )
        entity.rebuild(width, height)
        entities.append(entity)

    return spec, width, height, terrain, goals, entities


def draw_background(
    screen: pygame.Surface,
    width: int,
    height: int,
    cell: int,
    terrain: List[dict],
    goals: List[dict],
) -> None:
    screen.fill(COLORS["bg"])
    for x in range(width + 1):
        pygame.draw.line(screen, COLORS["grid"], (x * cell, 0), (x * cell, height * cell), 1)
    for y in range(height + 1):
        pygame.draw.line(screen, COLORS["grid"], (0, y * cell), (width * cell, y * cell), 1)

    for area in terrain:
        colour = map_patch_colour(area.get("kind", "plain"))
        x1, y1 = int(area.get("x1", 0)), int(area.get("y1", 0))
        x2, y2 = int(area.get("x2", x1)), int(area.get("y2", y1))
        rect = pygame.Rect(
            min(x1, x2) * cell,
            min(y1, y2) * cell,
            (abs(x2 - x1) + 1) * cell,
            (abs(y2 - y1) + 1) * cell,
        )
        pygame.draw.rect(screen, colour, rect)

    for area in goals:
        x1, y1 = int(area.get("x1", 0)), int(area.get("y1", 0))
        x2, y2 = int(area.get("x2", x1)), int(area.get("y2", y1))
        rect = pygame.Rect(
            min(x1, x2) * cell,
            min(y1, y2) * cell,
            (abs(x2 - x1) + 1) * cell,
            (abs(y2 - y1) + 1) * cell,
        )
        pygame.draw.rect(screen, COLORS["target_area"], rect)


def draw_units(
    screen: pygame.Surface,
    entities: List[EntitySpec],
    cell: int,
    font: pygame.font.Font,
    selected_entity: Optional[int],
) -> None:
    for ent in entities:
        for unit in ent.units:
            box = pygame.Rect(unit.x * cell, unit.y * cell, cell, cell)
            if ent.side == "BLUE":
                inner, edge = COLORS["blue"], COLORS["blue_e"]
            elif ent.side == "RED":
                inner, edge = COLORS["red"], COLORS["red_e"]
            else:
                inner, edge = COLORS["urban"], COLORS["grid"]

            pygame.draw.rect(screen, inner, box)
            pygame.draw.rect(screen, edge, box, 2)

        ax, ay = ent.anchor
        label = font.render(ent.name, True, COLORS["objective"])
        screen.blit(label, (ax * cell + 2, ay * cell + 2))

    if selected_entity is not None:
        ent = entities[selected_entity]
        for unit in ent.units:
            box = pygame.Rect(unit.x * cell, unit.y * cell, cell, cell)
            pygame.draw.rect(screen, COLORS["selected"], box, 2)


def build_units_by_cell(entities: List[EntitySpec]) -> Dict[Tuple[int, int], List[UnitSlot]]:
    mapping: Dict[Tuple[int, int], List[UnitSlot]] = {}
    for ent in entities:
        for unit in ent.units:
            mapping.setdefault((unit.x, unit.y), []).append(unit)
    return mapping


def rect_from_cells(
    x1: int, y1: int, x2: int, y2: int, width: int, height: int
) -> Tuple[int, int, int, int]:
    x1, y1 = clamp_point(x1, y1, width, height)
    x2, y2 = clamp_point(x2, y2, width, height)
    if x1 > x2:
        x1, x2 = x2, x1
    if y1 > y2:
        y1, y2 = y2, y1
    return x1, y1, x2, y2


def terrain_hit(terrain: List[dict], x: int, y: int, width: int, height: int) -> Optional[int]:
    for idx, area in enumerate(terrain):
        x1 = int(area.get("x1", 0))
        y1 = int(area.get("y1", 0))
        x2 = int(area.get("x2", x1))
        y2 = int(area.get("y2", y1))
        x1, y1, x2, y2 = rect_from_cells(x1, y1, x2, y2, width, height)
        if x1 <= x <= x2 and y1 <= y <= y2:
            return idx
    return None


def goal_hit(goals: List[dict], x: int, y: int, width: int, height: int) -> Optional[int]:
    for idx, area in enumerate(goals):
        x1 = int(area.get("x1", 0))
        y1 = int(area.get("y1", 0))
        x2 = int(area.get("x2", x1))
        y2 = int(area.get("y2", y1))
        x1, y1, x2, y2 = rect_from_cells(x1, y1, x2, y2, width, height)
        if x1 <= x <= x2 and y1 <= y <= y2:
            return idx
    return None


def save_scenario(
    path: str,
    spec: dict,
    entities: List[EntitySpec],
    terrain: List[dict],
    goals: List[dict],
) -> None:
    for ent in entities:
        raw = ent.raw
        raw["count"] = int(ent.count)
        ax, ay = ent.anchor
        raw["anchor"] = {"x": int(ax), "y": int(ay)}
    spec["terrain"] = terrain
    spec["goal"] = goals
    with open(path, "w", encoding="utf-8") as fh:
        json.dump(spec, fh, indent=2)


def main() -> None:
    parser = argparse.ArgumentParser(description="Edit TDGsim scenario.json")
    parser.add_argument("--map", help="Path to scenario.json")
    parser.add_argument("--cell", type=int, default=10, help="Pixel size of each map cell.")
    parser.add_argument("--fps", type=int, default=60, help="Render frames per second.")
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    repo_root = find_repo_root(script_dir)
    map_path = args.map or os.path.join(repo_root, "data", "scenario.json")
    if not os.path.isabs(map_path):
        map_path = os.path.join(repo_root, map_path)

    spec, width, height, terrain, goals, entities = load_scenario(map_path)
    units_by_cell = build_units_by_cell(entities)

    pygame.init()
    pygame.display.set_caption("TDGsim Scenario Editor")
    screen = pygame.display.set_mode((width * args.cell, height * args.cell))
    font = pygame.font.SysFont(None, max(14, args.cell // 2))
    hud_font = pygame.font.SysFont(None, 20)
    clock = pygame.time.Clock()

    selected_entity: Optional[int] = None
    drag_unit: Optional[UnitSlot] = None
    selected_terrain: Optional[int] = None
    selected_goal: Optional[int] = None
    mode = "entity"
    terrain_kind = TERRAIN_KINDS[0]
    drag_origin: Optional[Tuple[int, int]] = None
    drag_rect: Optional[Tuple[int, int, int, int]] = None
    drag_new_rect: Optional[Tuple[int, int]] = None
    dirty = False
    running = True

    def rebuild_entity(ent: EntitySpec) -> None:
        nonlocal units_by_cell
        ent.rebuild(width, height)
        units_by_cell = build_units_by_cell(entities)

    while running:
        clock.tick(args.fps)
        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                running = False
            elif ev.type == pygame.KEYDOWN:
                if ev.key in (pygame.K_ESCAPE, pygame.K_q):
                    running = False
                elif ev.key == pygame.K_e:
                    mode = "entity"
                elif ev.key == pygame.K_t:
                    mode = "terrain"
                elif ev.key == pygame.K_g:
                    mode = "goal"
                elif ev.key == pygame.K_s:
                    save_scenario(map_path, spec, entities, terrain, goals)
                    dirty = False
                elif ev.key == pygame.K_r:
                    spec, width, height, terrain, goals, entities = load_scenario(map_path)
                    units_by_cell = build_units_by_cell(entities)
                    selected_entity = None
                    drag_unit = None
                    selected_terrain = None
                    selected_goal = None
                    mode = "entity"
                    dirty = False
                elif ev.key == pygame.K_TAB and entities and mode == "entity":
                    if selected_entity is None:
                        selected_entity = 0
                    else:
                        selected_entity = (selected_entity + 1) % len(entities)
                elif mode == "entity" and selected_entity is not None:
                    ent = entities[selected_entity]
                    step = 5 if (ev.mod & pygame.KMOD_SHIFT) else 1
                    if ev.key in (pygame.K_LEFT, pygame.K_RIGHT, pygame.K_UP, pygame.K_DOWN):
                        dx = -step if ev.key == pygame.K_LEFT else step if ev.key == pygame.K_RIGHT else 0
                        dy = -step if ev.key == pygame.K_UP else step if ev.key == pygame.K_DOWN else 0
                        ax, ay = ent.anchor
                        ent.anchor = clamp_point(ax + dx, ay + dy, width, height)
                        rebuild_entity(ent)
                        dirty = True
                    elif ev.key == pygame.K_LEFTBRACKET:
                        ent.count = max(1, int(ent.count) - 1)
                        rebuild_entity(ent)
                        dirty = True
                    elif ev.key == pygame.K_RIGHTBRACKET:
                        ent.count = int(ent.count) + 1
                        rebuild_entity(ent)
                        dirty = True
                elif mode == "terrain":
                    if pygame.K_1 <= ev.key <= pygame.K_6:
                        terrain_kind = TERRAIN_KINDS[ev.key - pygame.K_1]
                        if selected_terrain is not None:
                            terrain[selected_terrain]["kind"] = terrain_kind
                            dirty = True
            elif ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 1:
                mx, my = ev.pos
                cx, cy = mx // args.cell, my // args.cell
                if mode == "entity":
                    hit = units_by_cell.get((cx, cy))
                    if hit:
                        drag_unit = hit[0]
                        selected_entity = drag_unit.entity_index
                elif mode == "terrain":
                    hit_idx = terrain_hit(terrain, cx, cy, width, height)
                    selected_terrain = hit_idx
                    if hit_idx is not None:
                        area = terrain[hit_idx]
                        x1, y1 = int(area.get("x1", 0)), int(area.get("y1", 0))
                        x2, y2 = int(area.get("x2", x1)), int(area.get("y2", y1))
                        drag_origin = (cx, cy)
                        drag_rect = (x1, y1, x2, y2)
                        terrain_kind = str(area.get("kind", terrain_kind))
                    else:
                        drag_new_rect = (cx, cy)
                elif mode == "goal":
                    hit_idx = goal_hit(goals, cx, cy, width, height)
                    selected_goal = hit_idx
                    if hit_idx is not None:
                        area = goals[hit_idx]
                        x1, y1 = int(area.get("x1", 0)), int(area.get("y1", 0))
                        x2, y2 = int(area.get("x2", x1)), int(area.get("y2", y1))
                        drag_origin = (cx, cy)
                        drag_rect = (x1, y1, x2, y2)
                    else:
                        drag_new_rect = (cx, cy)
            elif ev.type == pygame.MOUSEBUTTONUP and ev.button == 1:
                drag_unit = None
                if mode in ("terrain", "goal"):
                    mx, my = ev.pos
                    cx, cy = mx // args.cell, my // args.cell
                    if drag_new_rect is not None:
                        x1, y1 = drag_new_rect
                        x2, y2 = cx, cy
                        x1, y1, x2, y2 = rect_from_cells(x1, y1, x2, y2, width, height)
                        if mode == "terrain":
                            terrain.append({"kind": terrain_kind, "x1": x1, "y1": y1, "x2": x2, "y2": y2})
                            selected_terrain = len(terrain) - 1
                        else:
                            goals.append({"x1": x1, "y1": y1, "x2": x2, "y2": y2, "score": 0.0})
                            selected_goal = len(goals) - 1
                        dirty = True
                    drag_origin = None
                    drag_rect = None
                    drag_new_rect = None
            elif ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 3 and mode == "terrain":
                mx, my = ev.pos
                cx, cy = mx // args.cell, my // args.cell
                hit_idx = terrain_hit(terrain, cx, cy, width, height)
                if hit_idx is not None:
                    terrain.pop(hit_idx)
                    selected_terrain = None
                    dirty = True
            elif ev.type == pygame.MOUSEBUTTONDOWN and ev.button == 3 and mode == "goal":
                mx, my = ev.pos
                cx, cy = mx // args.cell, my // args.cell
                hit_idx = goal_hit(goals, cx, cy, width, height)
                if hit_idx is not None:
                    goals.pop(hit_idx)
                    selected_goal = None
                    dirty = True
            elif ev.type == pygame.MOUSEMOTION and drag_unit:
                mx, my = ev.pos
                cx, cy = mx // args.cell, my // args.cell
                ent = entities[drag_unit.entity_index]
                new_ax = cx - drag_unit.col
                new_ay = cy - drag_unit.row
                ent.anchor = clamp_point(new_ax, new_ay, width, height)
                rebuild_entity(ent)
                dirty = True
            elif ev.type == pygame.MOUSEMOTION and mode in ("terrain", "goal"):
                mx, my = ev.pos
                cx, cy = mx // args.cell, my // args.cell
                if drag_origin and drag_rect:
                    dx = cx - drag_origin[0]
                    dy = cy - drag_origin[1]
                    x1, y1, x2, y2 = drag_rect
                    x1, y1, x2, y2 = rect_from_cells(x1 + dx, y1 + dy, x2 + dx, y2 + dy, width, height)
                    if mode == "terrain" and selected_terrain is not None:
                        area = terrain[selected_terrain]
                        area["x1"], area["y1"], area["x2"], area["y2"] = x1, y1, x2, y2
                        dirty = True
                    elif mode == "goal" and selected_goal is not None:
                        area = goals[selected_goal]
                        area["x1"], area["y1"], area["x2"], area["y2"] = x1, y1, x2, y2
                        dirty = True

        draw_background(screen, width, height, args.cell, terrain, goals)
        draw_units(screen, entities, args.cell, font, selected_entity)

        if mode == "terrain" and selected_terrain is not None:
            area = terrain[selected_terrain]
            x1, y1 = int(area.get("x1", 0)), int(area.get("y1", 0))
            x2, y2 = int(area.get("x2", x1)), int(area.get("y2", y1))
            rect = pygame.Rect(
                min(x1, x2) * args.cell,
                min(y1, y2) * args.cell,
                (abs(x2 - x1) + 1) * args.cell,
                (abs(y2 - y1) + 1) * args.cell,
            )
            pygame.draw.rect(screen, COLORS["selected"], rect, 2)

        if mode == "goal" and selected_goal is not None:
            area = goals[selected_goal]
            x1, y1 = int(area.get("x1", 0)), int(area.get("y1", 0))
            x2, y2 = int(area.get("x2", x1)), int(area.get("y2", y1))
            rect = pygame.Rect(
                min(x1, x2) * args.cell,
                min(y1, y2) * args.cell,
                (abs(x2 - x1) + 1) * args.cell,
                (abs(y2 - y1) + 1) * args.cell,
            )
            pygame.draw.rect(screen, COLORS["selected"], rect, 2)

        if mode in ("terrain", "goal") and drag_new_rect is not None:
            mx, my = pygame.mouse.get_pos()
            cx, cy = mx // args.cell, my // args.cell
            x1, y1 = drag_new_rect
            x1, y1, x2, y2 = rect_from_cells(x1, y1, cx, cy, width, height)
            rect = pygame.Rect(
                min(x1, x2) * args.cell,
                min(y1, y2) * args.cell,
                (abs(x2 - x1) + 1) * args.cell,
                (abs(y2 - y1) + 1) * args.cell,
            )
            pygame.draw.rect(screen, COLORS["selected"], rect, 1)

        hud_lines = [
            f"Map: {os.path.basename(map_path)}",
            f"{'DIRTY' if dirty else 'Saved'}",
            f"Mode: {mode} (terrain kind: {terrain_kind})",
        ]
        if selected_entity is not None:
            ent = entities[selected_entity]
            ax, ay = ent.anchor
            hud_lines.append(
                f"Selected: {ent.name} side={ent.side} type={ent.kind} count={ent.count} anchor=({ax},{ay})"
            )
        else:
            hud_lines.append("Selected: none")
        if mode == "entity":
            hud_lines.append("S=Save R=Reload TAB=Next [/] Count Arrows=Move Drag=Move E/T/G=Mode Q=Quit")
        else:
            if mode == "terrain":
                hud_lines.append("Drag=New/Move 1-6=Kind RightClick=Delete E/T/G=Mode S=Save R=Reload Q=Quit")
            else:
                hud_lines.append("Drag=New/Move RightClick=Delete E/T/G=Mode S=Save R=Reload Q=Quit")

        for i, line in enumerate(hud_lines):
            surf = hud_font.render(line, True, COLORS["blue_e"])
            screen.blit(surf, (8, 8 + i * (hud_font.get_height() + 2)))

        pygame.display.flip()

    pygame.quit()


if __name__ == "__main__":
    main()
