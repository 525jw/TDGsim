# render_map.py
# pip install pygame
import csv, pygame, sys, os

# 코드 ↔ 이름/색 매핑
TERRAIN_NAMES = {0:"PLAIN", 1:"RIVER", 2:"ROAD", 3:"FOREST", 4:"HILL"}
TERRAIN_COLORS = {
    0:(210,210,210),  # PLAIN
    1:(100,160,255),  # RIVER
    2:(180,120, 60),  # ROAD
    3:( 60,140, 80),  # FOREST
    4:(170,170,120),  # HILL
}
TEAM_COLORS = {
    0:( 30,120,255),  # BLUE_INF
    1:(220, 60, 60),  # RED_INF
}

def load_terrain_csv(path):
    terrain = []
    with open(path, newline="") as f:
        rdr = csv.reader(f)
        for row in rdr:
            if not row: continue
            terrain.append([int(x) for x in row])
    h = len(terrain)
    w = len(terrain[0]) if h else 0
    return terrain, w, h

def load_entities_csv(path):
    ents = []
    with open(path, newline="") as f:
        rdr = csv.reader(f)
        header = next(rdr, None)
        # 허용 헤더: id,x,y,team  또는 없는 경우 숫자 4열
        for row in rdr if header else csv.reader(open(path, newline="")):
            if not row: continue
            if header:
                rid = int(row[0]); x=int(row[1]); y=int(row[2]); team=int(row[3])
            else:
                rid = int(row[0]); x=int(row[1]); y=int(row[2]); team=int(row[3])
            ents.append((rid, x, y, team))
    return ents

def draw(screen, terrain, ents, cell_px, camx, camy):
    sh, sw = screen.get_height(), screen.get_width()
    screen.fill((0,0,0))

    H = len(terrain)
    W = len(terrain[0]) if H else 0

    # 화면에 보이는 타일 범위만 그리기
    x0 = max(0, camx)
    y0 = max(0, camy)
    x1 = min(W-1, camx + sw//cell_px + 1)
    y1 = min(H-1, camy + sh//cell_px + 1)

    for y in range(y0, y1+1):
        row = terrain[y]
        py = (y - camy) * cell_px
        for x in range(x0, x1+1):
            px = (x - camx) * cell_px
            color = TERRAIN_COLORS.get(row[x], (255,0,255))
            pygame.draw.rect(screen, color, (px, py, cell_px, cell_px))

    # 엔티티: 1셀 점유 → 해당 셀을 팀 색으로 덮거나 테두리로 표현
    # 여기선 테두리 사각형
    for rid, x, y, team in ents:
        if x0 <= x <= x1 and y0 <= y <= y1:
            px = (x - camy if False else x - camx) * cell_px  # sanity
            py = (y - camy) * cell_px
            pygame.draw.rect(screen, TEAM_COLORS.get(team, (255,255,255)),
                             (px, py, cell_px, cell_px), width=0)

def main():
    if len(sys.argv) < 3:
        print("Usage: python render_map.py terrain.csv entities.csv [cell_px]")
        sys.exit(1)
    terrain_path = sys.argv[1]
    entities_path = sys.argv[2]
    cell_px = int(sys.argv[3]) if len(sys.argv) >= 4 else 2  # 1셀=2px 기본

    terrain, W, H = load_terrain_csv(terrain_path)
    entities = load_entities_csv(entities_path)

    pygame.init()
    screen = pygame.display.set_mode((1200, 800))
    clock = pygame.time.Clock()
    camx, camy = 0, 0

    running = True
    while running:
        for e in pygame.event.get():
            if e.type == pygame.QUIT:
                running = False
            elif e.type == pygame.KEYDOWN:
                if e.key in (pygame.K_EQUALS, pygame.K_PLUS):
                    cell_px = min(40, cell_px + 1)
                elif e.key == pygame.K_MINUS:
                    cell_px = max(1, cell_px - 1)

        # 패닝
        keys = pygame.key.get_pressed()
        step = max(1, 20 // cell_px)
        if keys[pygame.K_LEFT]:  camx = max(0, camx - step)
        if keys[pygame.K_RIGHT]: camx = min(max(0, W-1), camx + step)
        if keys[pygame.K_UP]:    camy = max(0, camy - step)
        if keys[pygame.K_DOWN]:  camy = min(max(0, H-1), camy + step)

        draw(screen, terrain, entities, cell_px, camx, camy)
        pygame.display.flip()
        clock.tick(60)

    pygame.quit()

if __name__ == "__main__":
    main()
