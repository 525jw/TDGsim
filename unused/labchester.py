import pygame
import random
import math
import heapq
import itertools
from enum import Enum, auto
from pygame.locals import (K_UP, K_DOWN, K_LEFT, K_RIGHT, K_ESCAPE, KEYDOWN, QUIT,)

pygame.init()

MAP_MIN_X, MAP_MAX_X = (0,1000)
MAP_MIN_Y, MAP_MAX_Y = (0,1000)

blue_pop, red_pop = (1000, 500)
world_tick = 0
default_vision = 400
default_accuracy = 0.8

screen = pygame.display.set_mode([MAP_MAX_X, MAP_MAX_Y]) 
txtFont = pygame.font.SysFont(None, 50)

""" Constants """

class Direction(Enum):
    S   =   (-1, 0 )
    N   =   ( 1, 0 )
    W   =   ( 0,-1 )
    E   =   ( 0, 1 )
    NW  =   (-1,-1 )
    NE  =   (-1, 1 )
    SW  =   ( 1,-1 )
    SE  =   ( 1, 1 )

class UnitSize(Enum):
    SOLDIER     = 1
    SQUAD       = 10
    PLATOON     = 40
    COMPANY     = 120
    BATTALION   = 600

""" Untility """

def euclidean_dist(x1, y1, x2 ,y2):
    return math.sqrt((x1 - x2) ** 2 + (y1 - y2) **2)

def get_soldiers_all(unitgroup):
    """ Recursively collect all Soldier units and return"""
    soldiers = []
    for unit in unitgroup.lower_units:
        if isinstance(unit, Soldier):
            soldiers.append(unit)
        elif isinstance(unit, UnitGroup):
            soldiers.extend(get_soldiers_all(unit))
    return soldiers

""" Scheduler """

class Scheduler:
    def __init__(self):
        self.event_queue = []  # (tick, event_instance) priority queue
        self.counter = itertools.count()

    def add(self, tick, event):
        count = next(self.counter)
        heapq.heappush(self.event_queue, (tick, count, event))

    def run(self):
        events_to_execute = []

        while self.event_queue and self.event_queue[0][0] <= world_tick:
            _, _, event = heapq.heappop(self.event_queue)
            events_to_execute.append(event)

        for event in events_to_execute:
            event.execute()

        # self.monitor_queue("Executed events")

    def monitor_queue(self, message):
        print(f"\n[{message}] Queue status at tick {world_tick}:")
        for tick, count, event in self.event_queue:
            print(f"  Tick: {tick}, Event: {type(event).__name__}, Sender: {event.sender.get_name() if event.sender else 'None'}")

scheduler = Scheduler()


""" Unit """

visited = set()

class Unit:
    def __init__(self):
        self.x = None
        self.y = None
        self.pop = None
        self.name = None
        self.higher_unit = None
        self.team = None
        self.state = None
        self.color = None

    def get_x(self):
        return self.x
    def get_y(self):
        return self.y
    def get_pop(self):
        return self.pop
    def get_name(self):
        return self.name
    def get_higher_unit(self):
        return self.higher_unit
    def get_team(self):
        return self.team
    def get_state(self):
        return self.state

    def set_x(self, x):
        self.x = x
    def set_y(self, y):
        self.y = y
    def set_pop(self, pop):
        self.pop = pop
    def set_name(self, name):
        self.name = name
    def set_higher_unit(self, higher_unit):
        self.higher_unit = higher_unit
    def set_team(self, team):
        self.team = team
    
    def set_color(self, color):
        self.color = color

    def transition(self, newstate):
        self.state = newstate

class UnitGroup(Unit):
    def __init__(self):
        super().__init__()
        # 배치 범위
        (self.x_min, self.x_max), (self.y_min, self.y_max) = (MAP_MIN_X, MAP_MAX_X), (MAP_MIN_Y, MAP_MAX_Y)
        # 예비 병력 수
        self.pop_reserve = None
        # 하위 유닛의 리스트
        self.lower_units = []

    # ====== 배치 범위 init ======
    def set_range_auto(self):
        base_size = 10
        ran = base_size + int(float(self.pop) * 0.5)

        x_min, x_max = max(MAP_MIN_X, self.x - ran), min(MAP_MAX_X, self.x + ran)
        y_min, y_max = max(MAP_MIN_Y, self.y - ran), min(MAP_MAX_Y, self.y + ran)

        if self.higher_unit:
            x_min = max(x_min, self.higher_unit.x_min)
            x_max = min(x_max, self.higher_unit.x_max)
            y_min = max(y_min, self.higher_unit.y_min)
            y_max = min(y_max, self.higher_unit.y_max)

        self.set_range(x_min, x_max, y_min, y_max)

    def set_range(self, x_min, x_max, y_min, y_max):
        self.x_min, self.x_max = (x_min, x_max)
        self.y_min, self.y_max = (y_min, y_max)

    # ====== 예비 병력 init =======
    def set_pop_reserve(self, pop_reserve):
        self.pop_reserve = pop_reserve

    # ====== 하위 유닛 init ======
    def is_collision(self, x, y):
        if (x,y) in visited:
            return True
        else:
            visited.add((x,y))
            return False
    
    def deploy_auto(self):
        attemps = 2000
        for _ in range(attemps):
            rx = random.randrange(self.x_min,self.x_max)
            ry = random.randrange(self.y_min,self.y_max)
            if not self.is_collision(rx, ry):
                return rx,ry
        print("Warning : Failed to deploy")
        return -1, -1

    def add_lower_units_all(self):
        attemps = 2000
        for _ in range(attemps):
            if self.pop_reserve <= 0:
                break
            self.add_lower_unit()
        #  Soldier는 range 필요없고, 하위 제대 편성 안하니까 예외처리한건데 불편, 나중에 수정바람
        if self.lower_units[0].pop != 1:
            for i in range(len(self.lower_units)):
                self.lower_units[i].add_lower_units_all()

    def add_lower_unit(self, lower_unit : Unit):
        x,y = self.deploy_auto()
        if x == -1:
            return
        lower_unit.set_x(x)
        lower_unit.set_y(y)
        #  Soldier는 range 필요없고, 하위 제대 편성 안하니까 예외처리한건데 불편, 나중에 수정바람
        if lower_unit.pop != 1:
            lower_unit.set_range_auto()
        lower_unit.set_higher_unit(self)

        self.pop_reserve-=lower_unit.pop
        self.lower_units.append(lower_unit)
    
class Soldier(Unit):
    def __init__(self):
        super().__init__()

        self.accuracy = default_accuracy
        self.vision = default_vision
        self.state = Ready(self)

        self.getShot = False
    
    def dead(self, deadList):
        if self.getShot:
            deadList.append(self)

class Squad(UnitGroup):
    def __init__(self):
        super().__init__()

    def add_lower_unit(self):
        lower_unit = Soldier()
        lower_unit.set_pop(UnitSize.SOLDIER.value)
        super().add_lower_unit(lower_unit)
 
class Platoon(UnitGroup):
    def __init__(self):
        super().__init__()

    def add_lower_unit(self):
        lower_unit = Squad()
        lower_unit.set_pop(UnitSize.SQUAD.value)
        lower_unit.set_pop_reserve(lower_unit.pop)
        super().add_lower_unit(lower_unit)

class Company(UnitGroup):
    def __init__(self):
        super().__init__()

    def add_lower_unit(self):
        lower_unit = Platoon()
        lower_unit.set_pop(UnitSize.PLATOON.value)
        lower_unit.set_pop_reserve(lower_unit.pop)
        super().add_lower_unit(lower_unit)

class Battalion(UnitGroup):
    def __init__(self):
        super().__init__()

    def add_lower_unit(self):
        lower_unit = Company()
        lower_unit.set_pop(UnitSize.COMPANY.value)
        lower_unit.set_pop_reserve(lower_unit.pop)
        super().add_lower_unit(lower_unit)

""" Event """

class Event():
    def __init__(self, sender : Soldier = None, receiver : Soldier = None, req_tick = None):
        self.sender = sender
        self.receiver = receiver
        # 해당 이벤트 수행에 필요한 시간
        self.req_tick  = req_tick
        
    def execute(self):
        pass

class Seek(Event):
    def __init__(self, sender):
        super().__init__(sender = sender, receiver = None, req_tick = 1)

    def execute(self): 
        enemy = blueteam.copy() if self.sender.get_team() == "red" else redteam.copy()
        to_remove = [target for target in enemy if euclidean_dist(self.sender.x, self.sender.y, target.x, target.y) > self.sender.vision]
        for target in to_remove:
            if target in enemy:
                enemy.remove(target)
        
        pygame.draw.circle(screen, self.sender.color, (self.sender.x, self.sender.y), default_vision, 1)
        # print(f"{self.sender.get_name()} is seeking. Found {len(enemy)} enemies.")
        
        if enemy:
            if not isinstance(self.sender.state, Engaged):
                self.sender.transition(Engaged(self.sender))
                self.sender.state.extend_enemy(enemy)
                # print(f"{self.sender.get_name()} transitioned to Engaged!")
            else:
                self.sender.state.extend_enemy(enemy)
        else:
            if not isinstance(self.sender.state, Ready):
                self.sender.transition(Ready(self.sender))
                
                    
class Attack(Event):
    def __init__(self, sender, receiver):
        super().__init__(sender=sender, receiver=receiver, req_tick = 1)

    def execute(self):
        if random.uniform(0, 1) <= self.sender.accuracy:
            # print(f"{self.sender.get_name()} is attacking!")
            pygame.draw.line(screen, self.sender.color, (self.sender.x, self.sender.y), (self.receiver.x, self.receiver.y), 1)
            self.receiver.getShot = True
            # pygame.draw.circle(screen, (0,0,0), (self.receiver.x, self.receiver.y), 3, 3)
            


""" State """

class State():
    def __init__(self, unit : Soldier):
        self.unit = unit
        # 해당 상태가 된 시간
        self.start_tick = world_tick
        # 해당 상태가 된 후 경과된 시간
        self.elapsed_tick = 0

    def event_maker(self,event : Event):
        scheduler.add(world_tick + event.req_tick, event)

class Ready(State):
    def __init__(self, unit):
        super().__init__(unit)
        self.unit.set_color((255,200,200) if self.unit.get_team() == "red" else (200,200,255))

    def event_maker(self):
        event = Seek(self.unit)
        super().event_maker(event)

class Engaged(State):
    def __init__(self, unit):
        super().__init__(unit)
        self.unit.set_color((255,0,0) if self.unit.get_team() == "red" else (0,0,255))
        self.enemy = []

    def extend_enemy(self, enemy):
        self.enemy = list(set(self.enemy + enemy))

    def event_maker(self):
        self.update_tick()
        if self.elapsed_tick >= 5:
            event = Seek(self.unit)
            super().event_maker(event)
        event = Attack(self.unit, random.choice(self.enemy))
        super().event_maker(event)
    
    def update_tick(self):
        if world_tick - self.start_tick != self.elapsed_tick:
            self.elapsed_tick = world_tick - self.start_tick
# class Wounded(State):
#     def __init__(self, unit):
#         super().__init__(unit)

class Dead(State):
    def __init__(self, unit):
        super().__init__(unit)
        unit.getShot = True

class MoveState(State):
    def __init__(self, unit, target_x, target_y):
        super().__init__(unit)
        self.target_x = target_x
        self.target_y = target_y
    
    def event_maker(self):
        if euclidean_dist(self.unit.x, self.unit.y, self.target_x, self.target_y) > 5:
            dx = self.target_x - self.unit.x
            dy = self.target_y - self.unit.y
            dist = math.sqrt(dx ** 2 + dy ** 2)
            step_size = min(10, dist)  # 이동 속도 조절
            self.unit.x += int((dx / dist) * step_size)
            self.unit.y += int((dy / dist) * step_size)
            pygame.draw.line(screen, (255, 255, 0), (self.unit.x, self.unit.y), (self.target_x, self.target_y), 1)  # 이동 경로 표시
            scheduler.add(world_tick + 1, MoveOrder(self.unit, self.target_x, self.target_y))
        else:
            self.unit.transition(Ready(self.unit))

class MoveOrder(Event):
    def __init__(self, sender, target_x, target_y):
        super().__init__(sender=sender, req_tick=1)
        self.target_x = target_x
        self.target_y = target_y
    
    def execute(self):
        if not isinstance(self.sender.state, MoveState):
            self.sender.transition(MoveState(self.sender, self.target_x, self.target_y))
        self.sender.state.event_maker()



def draw_all_boundaries(unitgroup):
    """ Recursively draw boundaries for the given UnitGroup and its lower units """
    # 현재 UnitGroup의 경계선 그리기
    x_min, x_max = unitgroup.x_min, unitgroup.x_max
    y_min, y_max = unitgroup.y_min, unitgroup.y_max
    pygame.draw.rect(screen, (0, 0, 0), pygame.Rect(x_min, y_min, x_max - x_min, y_max - y_min), 1)
    
    # 하위 UnitGroup에 대해 재귀적으로 경계선 그리기
    for unit in unitgroup.lower_units:
        if isinstance(unit, UnitGroup):
            draw_all_boundaries(unit)

# red/blue team 생성
blueteam = []
redteam = []
dead_blueteam=[]
dead_redteam=[]

padding = 100

blue_battalion = Battalion()
blue_battalion.set_pop(blue_pop)
blue_battalion.set_pop_reserve(blue_pop)
blue_battalion.set_range(MAP_MIN_X + padding, MAP_MAX_X - padding, int(MAP_MAX_Y/2) + padding, MAP_MAX_Y - padding)
blue_battalion.add_lower_units_all()

red_battalion = Battalion()
red_battalion.set_pop(red_pop)
red_battalion.set_pop_reserve(red_pop)
red_battalion.set_range(MAP_MIN_X + padding, MAP_MAX_X - padding, MAP_MIN_Y + padding, int(MAP_MAX_Y/2) - padding)
red_battalion.add_lower_units_all()

blueteam = get_soldiers_all(blue_battalion)
redteam = get_soldiers_all(red_battalion)

for i in range(len(blueteam)):
    blueteam[i].set_team("blue")
    blueteam[i].set_color((200,200,255))
    blueteam[i].set_name("b"+str(i))
for i in range(len(redteam)):
    redteam[i].set_team("red")
    redteam[i].set_color((255,200,200))
    redteam[i].set_name("r"+str(i))

blue_pop, red_pop = (len(blueteam),len(redteam ))

# screen/red/blue 출력
screen.fill((255, 255, 255))

# Blue team 경계선 그리기
draw_all_boundaries(blue_battalion)

# Red team 경계선 그리기
draw_all_boundaries(red_battalion)

print("blue/red")
print(len(blueteam), len(redteam))
    
for i in range(len(blueteam)):
    pygame.draw.circle(screen, blueteam[i].color, (blueteam[i].x, blueteam[i].y), 3)
for i in range(len(redteam)):
    pygame.draw.circle(screen, redteam[i].color, (redteam[i].x, redteam[i].y), 3)
pygame.display.flip()

# ====== running ======

# 마우스 선택 관련 변수
selected_soldiers = []
dragging = False
start_drag = (0, 0)
end_drag = (0, 0)

state = 0
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            # print("blue start/end/lost")
            # print(blue_pop,len(blueteam),len(blueteam)-blue_pop)
            # print("red start/end/lost")
            # print(red_pop,len(redteam),len(redteam)-red_pop)
            running = False
        
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 3:  # 우클릭 드래그 시작 (병사 선택)
                dragging = True
                start_drag = event.pos
            elif event.button == 1 and selected_soldiers:  # 좌클릭(이동 명령)
                target_x, target_y = event.pos
                formation_offset = 0
                for soldier in selected_soldiers:
                    scheduler.add(world_tick + 1, MoveOrder(soldier, target_x + formation_offset, target_y))
        
        elif event.type == pygame.MOUSEBUTTONUP and event.button == 3:  # 드래그 종료 및 병사 선택
            dragging = False
            end_drag = event.pos
            selected_soldiers = [s for s in blueteam if start_drag[0] <= s.x <= end_drag[0] and start_drag[1] <= s.y <= end_drag[1]]

        elif event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
            if state == 0:
                # screen/red/blue 출력
                screen.fill((255, 255, 255))
                for i in range(len(blueteam)):
                    pygame.draw.circle(screen, blueteam[i].color, (blueteam[i].x, blueteam[i].y), 3)
                for i in range(len(redteam)):
                    pygame.draw.circle(screen, redteam[i].color, (redteam[i].x, redteam[i].y), 3)
                
                if len(blueteam) == 0 or len(redteam) == 0 or world_tick > 100:
                    #if len(blueteam) == 0:
                    #    text = txtFont.render("Red team won!", True, (0,0,0))
                    #else:
                    #    text = txtFont.render("Blue team won!", True, (0,0,0))
                    #screen.blit(text, (100,100))
                    #pygame.display.flip()
                    exit()

                for i in range(len(blueteam)):
                    blueteam[i].state.event_maker()
                for i in range(len(redteam)):
                    redteam[i].state.event_maker()
                scheduler.run()
                world_tick+=1
                # print(world_tick)


                state = 1
            elif state == 1:
                for i in range(len(blueteam)):
                    blueteam[i].dead(dead_blueteam)
                for i in dead_blueteam:
                    if i in blueteam:
                        blueteam.remove(i)
                for i in range(len(redteam)):
                    redteam[i].dead(dead_redteam)
                for i in dead_redteam:
                    if i in redteam:
                        redteam.remove(i)    
                screen.fill((255, 255, 255))
                for i in range(len(blueteam)):
                    pygame.draw.circle(screen, blueteam[i].color, (blueteam[i].x, blueteam[i].y), 3)
                for i in range(len(redteam)):
                    pygame.draw.circle(screen, redteam[i].color, (redteam[i].x, redteam[i].y), 3)
                print(len(blueteam), len(redteam))
                
                state = 0
                if dragging:
                    pygame.draw.rect(screen, (255, 255, 0), (start_drag[0], start_drag[1], end_drag[0] - start_drag[0], end_drag[1] - start_drag[1]), 2)
                for soldier in selected_soldiers:
                     pygame.draw.circle(screen, (255, 255, 0), (soldier.x, soldier.y), 5, 1)
        
    pygame.display.flip()
    

pygame.quit()