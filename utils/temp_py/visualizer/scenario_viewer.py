#!/usr/bin/env python3
"""
Scenario viewer for TDGsim scenario.json (read-only).

Controls:
  - Q or ESC: quit
"""

from __future__ import annotations

import argparse
import json
import math
import os
from dataclasses import dataclass, field
from typing import List, Tuple

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
}


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
    x: int
    y: int


@dataclass
class EntitySpec:
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
            self.units.append(UnitSlot(x, y))


def load_scenario(path: str):
    with open(path, "r", encoding="utf-8") as fh:
        spec = json.load(fh)

    size = spec.get("size") or {}
    width = int(size.get("w", spec.get("w", 0)) or 0)
    height = int(size.get("h", spec.get("h", 0)) or 0)
    terrain = [t for t in spec.get("terrain", []) if isinstance(t, dict)]
    goals = [g for g in spec.get("goal", []) if isinstance(g, dict)]

    entities: List[EntitySpec] = []
    for ent in spec.get("entity", []):
        if not isinstance(ent, dict):
            continue
        name = ent.get("id") or ent.get("uid") or ent.get("name") or "entity"
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


def main() -> None:
    parser = argparse.ArgumentParser(description="View TDGsim scenario.json.")
    parser.add_argument("--map", help="Path to scenario.json")
    parser.add_argument("--cell", type=int, default=10, help="Pixel size of each map cell.")
    parser.add_argument("--fps", type=int, default=60, help="Render frames per second.")
    parser.add_argument("--out", help="PNG output path (default: scenario_view.png next to script)")
    args = parser.parse_args()

    script_dir = os.path.dirname(os.path.abspath(__file__))
    repo_root = find_repo_root(script_dir)
    map_path = args.map or os.path.join(repo_root, "data", "scenario.json")
    if not os.path.isabs(map_path):
        map_path = os.path.join(repo_root, map_path)
    out_path = args.out
    if not out_path:
        out_path = os.path.join(script_dir, "scenario_view.png")

    spec, width, height, terrain, goals, entities = load_scenario(map_path)

    pygame.init()
    pygame.display.set_caption("TDGsim Scenario Viewer")
    screen = pygame.display.set_mode((width * args.cell, height * args.cell))
    font = pygame.font.SysFont(None, max(14, args.cell // 2))
    hud_font = pygame.font.SysFont(None, 20)
    clock = pygame.time.Clock()

    running = True
    while running:
        clock.tick(args.fps)
        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                running = False
            elif ev.type == pygame.KEYDOWN:
                if ev.key in (pygame.K_ESCAPE, pygame.K_q):
                    running = False

        draw_background(screen, width, height, args.cell, terrain, goals)
        draw_units(screen, entities, args.cell, font)

        hud = hud_font.render(os.path.basename(map_path), True, COLORS["blue_e"])
        screen.blit(hud, (8, 8))
        pygame.display.flip()
        pygame.image.save(screen, out_path)
        running = False

    pygame.quit()


if __name__ == "__main__":
    main()
