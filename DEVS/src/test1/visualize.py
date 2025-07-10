import pygame, sys, re
from collections import deque
from pathlib import Path

# ───── 전장 설정 ─────
GRID_SIZE      = 5
CELL_PIXELS    = 120
MARGIN         = 40
WINDOW_SIZE    = GRID_SIZE * CELL_PIXELS + MARGIN * 2
EFFECT_LIFE_MS = 600
FONT_SIZE_BIG  = 28
FONT_SIZE_SM   = 20

BLUE   = ( 50, 110, 255)
RED    = (215,  60,  60)
BLACK  = ( 20,  20,  20)
WHITE  = (245, 245, 245)
YELLOW = (255, 210,  20)
GRID   = (160, 160, 160)

# ───── 병사 · 효과 객체 ─────
NUM_MAP = {"First": 1, "Second": 2, "Third": 3}
class Soldier:
    def __init__(self, name, team, pos):
        self.name, self.team, self.pos = name, team, pos
        self.alive  = True
        self.health = 100
        m = re.search(r"(First|Second|Third)", name)
        self.num = NUM_MAP.get(m.group(1), 0) if m else 0
    def color(self):
        if not self.alive: return BLACK
        return BLUE if self.team == "Blue" else RED

class Effect:
    def __init__(self, kind, **kw):
        self.kind, self.kw = kind, kw
        self.birth = pygame.time.get_ticks()

def grid_to_px(x, y):
    return (
        MARGIN + x * CELL_PIXELS + CELL_PIXELS // 2,
        MARGIN + y * CELL_PIXELS + CELL_PIXELS // 2,
    )

# ───── 로그 파서 ─────
CREATED = re.compile(r"\[(\w+)\] Created, pos\(x,y\) = \((\d+),(\d+)\)")
FIRE    = re.compile(r"\[(\w+)\] Fire , pos\(x,y\) = \((\d+),(\d+)\)")
DMG     = re.compile(
    r"\[(\w+)\] My location: \((\d+),(\d+)\).*Impact location: "
    r"\((\d+),(\d+)\).*Damaged\s*,\s*health\s*=\s*(\d+)"
)
DIED    = re.compile(r"\[(\w+)\] Died")
TIME    = re.compile(r"(?:Fire|Damage)\s*Turn\s*([0-9.]+)")

def parse_log(path):
    soldiers, events = {}, deque()
    with open(path, encoding="utf-8") as f:
        for idx, raw in enumerate(f, 1):
            line = raw.rstrip("\n")
            if m := CREATED.search(line):
                name, x, y = m.group(1), int(m.group(2)), int(m.group(3))
                soldiers[name] = Soldier(
                    name,
                    "Blue" if name.startswith("Blue") else "Red",
                    (x, y),
                )
            elif m := FIRE.search(line):
                shooter, tx, ty = m.group(1), int(m.group(2)), int(m.group(3))
                events.append(("fire", shooter, (tx, ty), idx, line))
            elif m := DMG.search(line):
                name = m.group(1)
                ix, iy = int(m.group(4)), int(m.group(5))
                hp = int(m.group(6))
                events.append(("damage", name, hp, (ix, iy), idx, line))
            elif m := DIED.search(line):
                events.append(("die", m.group(1), idx, line))
            elif m := TIME.search(line):
                events.append(("time", float(m.group(1)), idx, line))
    return soldiers, events

# ───── 메인 ─────
def main(log_path="./src/logs/log_world.txt"):
    soldiers, events = parse_log(log_path)
    pygame.init()
    screen = pygame.display.set_mode((WINDOW_SIZE, WINDOW_SIZE))
    pygame.display.set_caption("Battle Visualization  (SPACE → next)")
    font_big = pygame.font.SysFont("consolas", FONT_SIZE_BIG)
    font_sm  = pygame.font.SysFont("consolas", FONT_SIZE_SM)
    clock    = pygame.time.Clock()

    active        = []
    current_turn  = 0.0
    current_line  = "(log not started)"

    def play(evt):
        nonlocal current_turn, current_line
        kind, *rest = evt
        if kind == "time":
            current_turn, ln, txt = rest
            current_line = f"{ln:>4}: {txt}"
        elif kind == "fire":
            shooter, (tx, ty), ln, txt = rest
            current_line = f"{ln:>4}: {txt}"
            if shooter in soldiers:
                sx, sy = soldiers[shooter].pos
                col    = soldiers[shooter].color()
                active.append(
                    Effect(
                        "line",
                        start=grid_to_px(sx, sy),
                        end=grid_to_px(tx, ty),
                        color=col,
                    )
                )
        elif kind == "damage":
            name, hp, (ix, iy), ln, txt = rest
            current_line = f"{ln:>4}: {txt}"
            if name in soldiers:
                soldiers[name].health = hp
            active.append(Effect("circle", center=grid_to_px(ix, iy)))
        elif kind == "die":
            dead, ln, txt = rest
            current_line = f"{ln:>4}: {txt}"
            if dead in soldiers:
                soldiers[dead].alive = False
                soldiers[dead].health = 0

    # ─ 루프 ─
    while True:
        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                pygame.quit(); sys.exit()
            elif ev.type == pygame.KEYDOWN and ev.key == pygame.K_SPACE:
                if events:
                    play(events.popleft())

        now = pygame.time.get_ticks()
        active[:] = [e for e in active if now - e.birth <= EFFECT_LIFE_MS]

        screen.fill(WHITE)
        # Turn & Log
        screen.blit(
            font_big.render(f"Turn : {current_turn:.1f}", True, BLACK),
            (MARGIN, MARGIN // 3),
        )
        screen.blit(
            font_sm.render(current_line, True, BLACK),
            (MARGIN, MARGIN // 3 + FONT_SIZE_BIG + 6),
        )

        # 격자
        for i in range(GRID_SIZE + 1):
            x = MARGIN + i * CELL_PIXELS
            pygame.draw.line(screen, GRID, (x, MARGIN), (x, WINDOW_SIZE - MARGIN))
            y = MARGIN + i * CELL_PIXELS
            pygame.draw.line(screen, GRID, (MARGIN, y), (WINDOW_SIZE - MARGIN, y))

        # 병사 + 번호 + Health
        for s in soldiers.values():
            cx, cy = grid_to_px(*s.pos)
            pygame.draw.circle(screen, s.color(), (cx, cy), 18)
            num_text = font_sm.render(str(s.num), True, WHITE)
            screen.blit(num_text, num_text.get_rect(center=(cx, cy)))
            hp_text = font_sm.render(str(s.health), True, BLACK)
            screen.blit(hp_text, hp_text.get_rect(center=(cx, cy + 30)))

        # 효과
        for e in active:
            if e.kind == "line":
                pygame.draw.line(
                    screen, e.kw["color"], e.kw["start"], e.kw["end"], 4
                )
            else:
                pygame.draw.circle(screen, YELLOW, e.kw["center"], 24, 4)

        pygame.display.flip()
        clock.tick(60)

if __name__ == "__main__":
    path = "./src/logs/log_world.txt"
    if not Path(path).exists():
        print(f"{path} not found! 동일 폴더에 로그 파일을 놓아주십시오.")
        sys.exit(1)
    main(path)
