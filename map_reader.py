#!/usr/bin/env python3
import json, os, pygame

COLORS = {
    "bg": (247,247,247), "grid": (200,200,200),
    "water": (158,202,225), "forest": (161,217,155),
    "urban": (189,189,189), "rough": (231,186,82),
    "blue": (49,130,189), "blue_e": (8,81,156),
    "red": (222,45,38), "red_e": (165,15,21),
    "white": (255,255,255)
}

def glyph(t: str) -> str:
    t = t.lower()
    if t.startswith("rifle") or t.startswith("inf"): return "R"
    if t.startswith("tank") or t.startswith("arm"): return "T"
    return t[:3].upper()

def main():
    # map.json 자동 로드
    spec_path = os.path.join(os.path.dirname(__file__), "map.json")
    with open(spec_path, "r", encoding="utf-8") as f:
        spec = json.load(f)

    W, H = spec["w"], spec["h"]
    patches, units = spec.get("patches", []), spec.get("units", [])

    # === 여기서 윈도우 크기를 맵 크기에 맞게 자동 설정 ===
    cell = 4  # 셀 크기(px)
    WIN_W, WIN_H = W * cell, H * cell

    pygame.init()
    screen = pygame.display.set_mode((WIN_W, WIN_H))
    pygame.display.set_caption("Full Map Viewer")
    font = pygame.font.SysFont(None, max(14, cell//3))
    clock = pygame.time.Clock()

    running = True
    while running:
        for e in pygame.event.get():
            if e.type == pygame.QUIT: running = False
            elif e.type == pygame.KEYDOWN and e.key in (pygame.K_ESCAPE, pygame.K_q):
                running = False

        # 배경+그리드
        screen.fill(COLORS["bg"])
        for x in range(W+1):
            pygame.draw.line(screen, COLORS["grid"], (x*cell,0),(x*cell,H*cell),1)
        for y in range(H+1):
            pygame.draw.line(screen, COLORS["grid"], (0,y*cell),(W*cell,y*cell),1)

        # 패치
        for p in patches:
            x1,y1,x2,y2 = p["x1"],p["y1"],p["x2"],p["y2"]
            rect = pygame.Rect(min(x1,x2)*cell, min(y1,y2)*cell,
                               (abs(x2-x1)+1)*cell, (abs(y2-y1)+1)*cell)
            color = COLORS.get(p["kind"], (220,220,220))
            pygame.draw.rect(screen, color, rect)

        # 유닛
        for u in units:
            x,y = u["x"], u["y"]
            box = pygame.Rect(x*cell, y*cell, cell, cell)
            if u["side"].upper()=="BLUE": fill,edge = COLORS["blue"],COLORS["blue_e"]
            else: fill,edge = COLORS["red"],COLORS["red_e"]
            pygame.draw.rect(screen, fill, box)
            pygame.draw.rect(screen, edge, box, 2)
            tag = glyph(u["type"])
            txt = font.render(tag, True, COLORS["white"])
            screen.blit(txt, (x*cell+(cell-txt.get_width())//2,
                              y*cell+(cell-txt.get_height())//2))

        pygame.display.flip()
        clock.tick(30)

    pygame.quit()

if __name__=="__main__":
    main()
