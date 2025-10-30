#!/usr/bin/env python3
import argparse
import json
import os
from dataclasses import dataclass
from typing import List, Tuple, Optional, Dict

import pygame

# ---------------------- Colors ----------------------
COLORS = {
    "bg": (247, 247, 247),
    "grid": (210, 210, 210),
    "water": (158, 202, 225),
    "bridge": (205, 170, 102),
    "unit_blue": (49, 130, 189),
    "unit_red": (222, 45, 38),
    "black": (32, 32, 32),
    "white": (255, 255, 255),
    "hover": (0, 0, 0),
}

# ---------------------- Data ----------------------
@dataclass
class Patch:
    kind: str  # "water" | "bridge"
    x1: int; y1: int; x2: int; y2: int

    def rect_norm(self) -> Tuple[int, int, int, int]:
        x1, x2 = sorted((self.x1, self.x2))
        y1, y2 = sorted((self.y1, self.y2))
        return x1, y1, x2, y2

    def hit(self, gx: int, gy: int) -> bool:
        x1, y1, x2, y2 = self.rect_norm()
        return x1 <= gx <= x2 and y1 <= gy <= y2

@dataclass
class Unit:
    uid: str   # e.g., BLUE-PLT1-SOL01 or BLUE-ART
    side: str  # "BLUE" | "RED"
    type: str  # "rifle" | "artillery"
    x: int; y: int

# ---------------------- Utils ----------------------
def clamp(v, lo, hi):
    return max(lo, min(hi, v))

def cell_from_px(mx: int, my: int, cell: int) -> Tuple[int, int]:
    return mx // cell, my // cell

def px_from_cell(gx: int, gy: int, cell: int) -> Tuple[int, int]:
    return gx * cell, gy * cell

def normalize_rect(a: Tuple[int,int], b: Tuple[int,int]) -> Tuple[int,int,int,int]:
    (x0, y0), (x1, y1) = a, b
    x1, x2 = sorted((x0, x1))
    y1, y2 = sorted((y0, y1))
    return x1, y1, x2, y2

def next_sol_index(units: List[Unit], side: str, platoon: int) -> int:
    """Return next SOL nn (int) for side+platoon."""
    prefix = f"{side}-PLT{platoon}-SOL"
    used = []
    for u in units:
        if u.uid.startswith(prefix) and u.type == "rifle":
            try:
                nn = int(u.uid.split("-")[-1].replace("SOL","").replace(f"{platoon}-",""))
            except Exception:
                # more robust parse
                tail = u.uid.split("-")[-1]
                if tail.startswith("SOL"):
                    nn = int(tail[3:])
                else:
                    continue
            used.append(nn)
    nn = 1
    while nn in used:
        nn += 1
    return nn

def has_artillery(units: List[Unit]) -> bool:
    return any(u.type == "artillery" for u in units)

# ---------------------- IO ----------------------
def save_json(path: str, w: int, h: int, patches: List[Patch], units: List[Unit]) -> None:
    obj = {
        "w": w,
        "h": h,
        "patches": [
            {"kind": p.kind, "x1": p.x1, "y1": p.y1, "x2": p.x2, "y2": p.y2}
            for p in patches
        ],
        "units": [
            {"uid": u.uid, "side": u.side, "type": u.type, "x": u.x, "y": u.y}
            for u in units
        ],
    }
    with open(path, "w", encoding="utf-8") as f:
        json.dump(obj, f, ensure_ascii=False, separators=(",",":"))
    print(f"[saved] {path}")

def load_json(path: str) -> Tuple[int, int, List[Patch], List[Unit]]:
    with open(path, "r", encoding="utf-8") as f:
        obj = json.load(f)
    w = int(obj["w"]); h = int(obj["h"])
    patches = [Patch(p["kind"], int(p["x1"]), int(p["y1"]), int(p["x2"]), int(p["y2"]))
               for p in obj.get("patches", [])]
    units = [Unit(u["uid"], u["side"], u["type"], int(u["x"]), int(u["y"]))
             for u in obj.get("units", [])]
    return w, h, patches, units

# ---------------------- Main Editor ----------------------
class Editor:
    def __init__(self, w: int, h: int, cell: int, start_mode: str = "water"):
        self.w = w; self.h = h
        self.cell = cell
        self.min_cell, self.max_cell = 6, 80

        self.patches: List[Patch] = []
        self.units: List[Unit] = []

        # UI state
        self.mode = start_mode  # "water" | "bridge" | "unit_rifle" | "unit_artillery"
        self.side = "BLUE"
        self.platoon = 1

        self.dragging_rect = False
        self.rect_start_cell: Optional[Tuple[int,int]] = None

        self.dragging_unit: Optional[int] = None  # index in self.units
        self.hover_patch: Optional[int] = None
        self.hover_unit: Optional[int] = None

        pygame.init()
        self.screen = pygame.display.set_mode((w * cell, h * cell))
        pygame.display.set_caption("TDG Map Editor")
        self.font = pygame.font.SysFont(None, 18)
        self.hud_font = pygame.font.SysFont(None, 22)
        self.clock = pygame.time.Clock()

    # ---------- Drawing ----------
    def draw_grid(self):
        s = self.screen; c = self.cell
        s.fill(COLORS["bg"])
        for x in range(self.w + 1):
            pygame.draw.line(s, COLORS["grid"], (x * c, 0), (x * c, self.h * c))
        for y in range(self.h + 1):
            pygame.draw.line(s, COLORS["grid"], (0, y * c), (self.w * c, y * c))

    def draw_patches(self):
        s = self.screen; c = self.cell
        for idx, p in enumerate(self.patches):
            x1, y1, x2, y2 = p.rect_norm()
            rect = pygame.Rect(x1 * c, y1 * c, (x2 - x1 + 1) * c, (y2 - y1 + 1) * c)
            col = COLORS["water"] if p.kind == "water" else COLORS["bridge"]
            pygame.draw.rect(s, col, rect)
            if self.hover_patch == idx:
                pygame.draw.rect(s, COLORS["hover"], rect, 2)

    def draw_units(self):
        s = self.screen; c = self.cell
        for idx, u in enumerate(self.units):
            x, y = u.x, u.y
            box = pygame.Rect(x * c, y * c, c, c)
            col = COLORS["unit_blue"] if u.side == "BLUE" else COLORS["unit_red"]
            pygame.draw.rect(s, col, box)
            pygame.draw.rect(s, COLORS["black"], box, 2)
            label = self.font.render(("A" if u.type == "artillery" else "R"), True, COLORS["white"])
            s.blit(label, (x * c + (c - label.get_width()) // 2,
                           y * c + (c - label.get_height()) // 2))
            if self.hover_unit == idx:
                pygame.draw.rect(s, COLORS["hover"], box, 2)

    def draw_drag_preview(self, mouse_cell: Tuple[int,int]):
        if self.mode in ("water", "bridge") and self.dragging_rect and self.rect_start_cell:
            c = self.cell
            x1, y1, x2, y2 = normalize_rect(self.rect_start_cell, mouse_cell)
            rect = pygame.Rect(x1 * c, y1 * c, (x2 - x1 + 1) * c, (y2 - y1 + 1) * c)
            col = COLORS["water"] if self.mode == "water" else COLORS["bridge"]
            pygame.draw.rect(self.screen, col, rect, 2)

    def draw_hud(self):
        lines = [
            f"Mode: {self.mode} | Team: {self.side} | (rifle click=spawn 5x5 PLT auto-numbered)",
            "Keys: 1=water  2=bridge  3=unit-rifle(5x5)  4=unit-artillery",
            "      B/R=team  S=save  L=load  +/-=zoom  ESC/Q=quit",
            "Mouse: L-drag=draw rect / move unit   R-click=delete (patch/unit)",
        ]

        y = 6
        for line in lines:
            surf = self.hud_font.render(line, True, COLORS["black"])
            self.screen.blit(surf, (6, y))
            y += self.hud_font.get_height() + 2

    # ---------- Hit tests ----------
    def find_patch_at(self, gx: int, gy: int) -> Optional[int]:
        for i in reversed(range(len(self.patches))):
            if self.patches[i].hit(gx, gy):
                return i
        return None

    def find_unit_at(self, gx: int, gy: int) -> Optional[int]:
        for i in reversed(range(len(self.units))):
            u = self.units[i]
            if u.x == gx and u.y == gy:
                return i
        return None

    # ---------- Actions ----------
    def add_patch(self, kind: str, a: Tuple[int,int], b: Tuple[int,int]):
        x1, y1, x2, y2 = normalize_rect(a, b)
        x1 = clamp(x1, 0, self.w-1); x2 = clamp(x2, 0, self.w-1)
        y1 = clamp(y1, 0, self.h-1); y2 = clamp(y2, 0, self.h-1)
        self.patches.append(Patch(kind, x1, y1, x2, y2))

    def delete_patch(self, idx: int):
        del self.patches[idx]

    def add_unit_rifle(self, gx: int, gy: int):
        n = next_sol_index(self.units, self.side, self.platoon)
        uid = f"{self.side}-PLT{self.platoon}-SOL{n:02d}"
        self.units.append(Unit(uid=uid, side=self.side, type="rifle", x=gx, y=gy))

    def add_unit_artillery(self, gx: int, gy: int):
        if has_artillery(self.units):
            return  # 딱 하나만
        uid = f"{self.side}-ART"
        self.units.append(Unit(uid=uid, side=self.side, type="artillery", x=gx, y=gy))

    def delete_unit(self, idx: int):
        del self.units[idx]

    def start_drag_unit(self, idx: int):
        self.dragging_unit = idx

    def move_drag_unit(self, gx: int, gy: int):
        if self.dragging_unit is None:
            return
        u = self.units[self.dragging_unit]
        u.x = clamp(gx, 0, self.w-1)
        u.y = clamp(gy, 0, self.h-1)

    def end_drag_unit(self):
        self.dragging_unit = None

    def set_mode(self, m: str):
        self.mode = m

    def zoom(self, delta: int):
        new_cell = clamp(self.cell + delta, self.min_cell, self.max_cell)
        if new_cell != self.cell:
            self.cell = new_cell
            self.screen = pygame.display.set_mode((self.w * self.cell, self.h * self.cell))
            # font sizes keep readable
            self.font = pygame.font.SysFont(None, max(14, self.cell // 2))
            self.hud_font = pygame.font.SysFont(None, 22)
    
    def parse_platoon_id(self, uid: str, side: str) -> Optional[int]:
        """
        uid가 '{SIDE}-PLT{n}-SOL{nn}'이면 n을 반환. 아니면 None.
        """
        try:
            if not uid.startswith(f"{side}-PLT"):
                return None
            # ex) BLUE-PLT2-SOL07
            parts = uid.split("-")
            for p in parts:
                if p.startswith("PLT"):
                    return int(p[3:])
        except Exception:
            pass
        return None

    def next_platoon_index(self, units: List["Unit"], side: str) -> int:
        """
        해당 side의 기존 보병(=rifle) 플래툰 번호들 중 최댓값 + 1을 반환.
        하나도 없으면 1.
        """
        seen = set()
        for u in units:
            if u.type != "rifle" or u.side != side:
                continue
            n = self.parse_platoon_id(u.uid, side)
            if n is not None:
                seen.add(n)
        return (max(seen) + 1) if seen else 1

    def add_platoon_rifle_5x5(self, gx: int, gy: int):
        """
        클릭 지점 기준으로 5x5 보병을 한 번에 추가.
        - 해당 팀의 다음 PLT 번호를 자동 배정
        - SOL01..SOL25로 고정
        - 클릭이 가장 왼쪽-위 모서리가 되도록 하되, 맵 경계 밖이면 자동 보정
        """
        # 맵 경계 내 5x5 블록이 들어가도록 시작점 보정
        start_x = clamp(gx, 0, self.w - 5)
        start_y = clamp(gy, 0, self.h - 5)

        platoon_id = self.next_platoon_index(self.units, self.side)

        sol = 1
        for dy in range(5):
            for dx in range(5):
                uid = f"{self.side}-PLT{platoon_id}-SOL{sol:02d}"
                self.units.append(Unit(uid=uid,
                                       side=self.side,
                                       type="rifle",
                                       x=start_x + dx,
                                       y=start_y + dy))
                sol += 1


    # ---------- Loop ----------
    def run(self, path: str):
        running = True
        while running:
            dt = self.clock.tick(60)
            mx, my = pygame.mouse.get_pos()
            gx, gy = cell_from_px(mx, my, self.cell)
            gx = clamp(gx, 0, self.w-1); gy = clamp(gy, 0, self.h-1)

            # Hover detection
            self.hover_patch = self.find_patch_at(gx, gy)
            self.hover_unit = self.find_unit_at(gx, gy)

            for ev in pygame.event.get():
                if ev.type == pygame.QUIT:
                    running = False
                elif ev.type == pygame.KEYDOWN:
                    if ev.key in (pygame.K_ESCAPE, pygame.K_q):
                        running = False
                    elif ev.key == pygame.K_1:
                        self.set_mode("water")
                    elif ev.key == pygame.K_2:
                        self.set_mode("bridge")
                    elif ev.key == pygame.K_3:
                        self.set_mode("unit_rifle")
                    elif ev.key == pygame.K_4:
                        self.set_mode("unit_artillery")
                    elif ev.key == pygame.K_b:
                        self.side = "BLUE"
                    elif ev.key == pygame.K_r:
                        # 주의: R 키는 여기선 side RED 전환. (재생기에서 R=reset과 충돌 없음)
                        self.side = "RED"
                    elif ev.key == pygame.K_LEFTBRACKET:
                        self.platoon = max(1, self.platoon - 1)
                    elif ev.key == pygame.K_RIGHTBRACKET:
                        self.platoon = min(99, self.platoon + 1)
                    elif ev.key == pygame.K_s:
                        save_json(path, self.w, self.h, self.patches, self.units)
                    elif ev.key == pygame.K_l:
                        if os.path.isfile(path):
                            self.w, self.h, self.patches, self.units = load_json(path)
                            self.screen = pygame.display.set_mode((self.w * self.cell, self.h * self.cell))
                    elif ev.key in (pygame.K_PLUS, pygame.K_EQUALS, pygame.K_KP_PLUS):
                        self.zoom(+2)
                    elif ev.key in (pygame.K_MINUS, pygame.K_KP_MINUS):
                        self.zoom(-2)

                elif ev.type == pygame.MOUSEBUTTONDOWN:
                    if ev.button == 1:  # left
                        if self.mode in ("water", "bridge"):
                            self.dragging_rect = True
                            self.rect_start_cell = (gx, gy)
                        elif self.mode.startswith("unit"):
                            # 유닛 드래그 우선
                            if self.hover_unit is not None:
                                self.start_drag_unit(self.hover_unit)
                            else:
                                if self.mode == "unit_rifle":
                                    # 한 번 클릭에 5x5 보병 분대 자동 생성 (PLT 자동 증가)
                                    self.add_platoon_rifle_5x5(gx, gy)
                                else:
                                    self.add_unit_artillery(gx, gy)

                    elif ev.button == 3:  # right -> delete
                        if self.hover_unit is not None:
                            self.delete_unit(self.hover_unit)
                        elif self.hover_patch is not None:
                            self.delete_patch(self.hover_patch)

                elif ev.type == pygame.MOUSEBUTTONUP:
                    if ev.button == 1:
                        if self.dragging_unit is not None:
                            self.end_drag_unit()
                        elif self.dragging_rect and self.rect_start_cell:
                            kind = "water" if self.mode == "water" else "bridge"
                            self.add_patch(kind, self.rect_start_cell, (gx, gy))
                            self.dragging_rect = False
                            self.rect_start_cell = None

                elif ev.type == pygame.MOUSEMOTION:
                    if self.dragging_unit is not None:
                        self.move_drag_unit(gx, gy)

            # Draw
            self.draw_grid()
            self.draw_patches()
            self.draw_units()
            self.draw_drag_preview((gx, gy))
            self.draw_hud()
            pygame.display.flip()

        pygame.quit()

# ---------------------- CLI ----------------------
def main():
    parser = argparse.ArgumentParser(description="Simple TDG map editor (pygame).")
    parser.add_argument("--map", default="new_map.json", help="Path to map.json (save/load).")
    parser.add_argument("--w", type=int, default=100, help="Grid width (cells).")
    parser.add_argument("--h", type=int, default=150, help="Grid height (cells).")
    parser.add_argument("--cell", type=int, default=8, help="Cell pixel size.")
    args = parser.parse_args()

    editor = Editor(w=args.w, h=args.h, cell=args.cell)
    # 파일이 있으면 불러와서 동일 스키마 유지
    if os.path.isfile(args.map):
        try:
            w, h, patches, units = load_json(args.map)
            editor.w, editor.h = w, h
            editor.patches = patches
            editor.units = units
            editor.screen = pygame.display.set_mode((w * editor.cell, h * editor.cell))
        except Exception as e:
            print(f"[warn] failed to load {args.map}: {e}")

    editor.run(args.map)

if __name__ == "__main__":
    main()
