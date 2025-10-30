#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
crossval.py — Count BLUE/RED deaths from a simulation log and detect duplicates.

Usage:
  python crossval.py                       # default: ./log_simulation.txt
  python crossval.py -f logs/log_simulation.txt
  python crossval.py -f log.txt -v         # show lines with 'dead' but no side
"""

from __future__ import annotations
import argparse
import re
from collections import Counter
from dataclasses import dataclass
from pathlib import Path
from typing import List, Optional, Tuple

# -------- Settings --------
DEF_PATH = Path("logs/log_simulation.txt")   # 기본값
# --------------------------

# Regexes (case-insensitive)
RE_DEAD       = re.compile(r"\bdead\b", re.IGNORECASE)
RE_SIDE_TAG   = re.compile(r"\bside\s*[:=]\s*(BLUE|RED)\b", re.IGNORECASE)
RE_SIDE_WORD  = re.compile(r"\b(BLUE|RED)\b", re.IGNORECASE)

# 다양한 UID 키를 지원 (로그 포맷에 맞춰 추가/삭제 가능)
RE_UID_TOKEN  = re.compile(
    r"\b(id|uid|entityId|entity|unit|targetId|victimId|name)\s*[:=]\s*([A-Za-z0-9._\-]+)",
    re.IGNORECASE
)

@dataclass
class DeathEvent:
    side: Optional[str]  # 'BLUE' | 'RED' | None
    uid:  Optional[str]  # 식별자(있으면 중복 판정 가능)


def _nearest_token_value(anchor_pos: int, tokens: List[Tuple[str, int]]) -> Optional[str]:
    """anchor_pos에 가장 가까운 토큰의 값을 반환. tokens는 (value, start_pos) 리스트."""
    if not tokens:
        return None
    val, _ = min(tokens, key=lambda t: abs(t[1] - anchor_pos))
    return val


def _extract_death_events_from_line(line: str) -> List[DeathEvent]:
    """
    한 줄에서 'dead' 발생마다 DeathEvent 생성.
    - side는 우선 'side=BLUE/RED' 태그를 사용.
    - 없으면 BLUE/RED 단어 중 dead와 가장 가까운 토큰으로 추정.
    - uid는 다양한 키(id|uid|entityId|unit|targetId|victimId|name 등) 중 dead와 가장 가까운 값을 사용.
    """
    events: List[DeathEvent] = []
    dead_matches = list(RE_DEAD.finditer(line))
    if not dead_matches:
        return events

    # side 결정용 토큰들
    side_tag = RE_SIDE_TAG.search(line)
    side_tokens = [(m.group(1).upper(), m.start()) for m in RE_SIDE_WORD.finditer(line)]

    # uid 후보 토큰들
    uid_tokens = [(m.group(2), m.start(2)) for m in RE_UID_TOKEN.finditer(line)]

    for dm in dead_matches:
        dpos = dm.start()

        # side 결정
        if side_tag:
            side = side_tag.group(1).upper()
        else:
            side = _nearest_token_value(dpos, side_tokens)
            if side is not None:
                side = side.upper()

        # uid 결정 (dead와 가장 가까운 uid 토큰)
        uid = _nearest_token_value(dpos, uid_tokens)

        events.append(DeathEvent(side=side, uid=uid))

    return events


def count_deaths(path: Path, verbose: bool = False) -> tuple[Counter, int, list[tuple[int, str]]]:
    """
    Returns:
      (counts, duplicate_dead, unmatched_lines)
      - counts: {'BLUE': n, 'RED': m}
      - duplicate_dead: 같은 uid에서 둘째 이후 dead 발생 횟수
      - unmatched_lines: 'dead'가 있으나 side를 특정 못한 라인 목록 [(lineno, text), ...]
    """
    counts = Counter()
    duplicate_dead = 0
    unmatched_lines: list[tuple[int, str]] = []
    seen_dead_uids: set[str] = set()

    with path.open("r", encoding="utf-8", errors="ignore") as f:
        for lineno, raw in enumerate(f, start=1):
            line = raw.rstrip("\n")
            if not RE_DEAD.search(line):
                continue

            events = _extract_death_events_from_line(line)
            if not events:
                continue

            # side 집계 + duplicate 판정
            for ev in events:
                if ev.side in ("BLUE", "RED"):
                    counts[ev.side] += 1
                else:
                    # side를 특정 못한 경우, 추후 디버깅 위해 저장(옵션 -v)
                    unmatched_lines.append((lineno, line))

                # UID 중복 체크
                if ev.uid:
                    if ev.uid in seen_dead_uids:
                        duplicate_dead += 1
                    else:
                        seen_dead_uids.add(ev.uid)

    # unmatched_lines에는 같은 라인이 여러 번 들어갈 수 있어 dedup 처리
    if verbose and unmatched_lines:
        # 같은 줄이 여러 번 들어갔을 수 있으니 정규화
        seen = set()
        deduped = []
        for ln, txt in unmatched_lines:
            key = (ln, txt)
            if key not in seen:
                seen.add(key)
                deduped.append((ln, txt))
        unmatched_lines = deduped

    return counts, duplicate_dead, unmatched_lines


def main():
    ap = argparse.ArgumentParser(description="Count BLUE/RED deaths (and duplicate deaths) from a simulation log.")
    ap.add_argument("-f", "--file", default=str(DEF_PATH), help="Path to log file (default: logs/log_simulation.txt)")
    ap.add_argument("-v", "--verbose", action="store_true", help="Show lines that contain 'dead' but no BLUE/RED could be inferred")
    args = ap.parse_args()

    path = Path(args.file)
    if not path.exists():
        print(f"[ERROR] File not found: {path}")
        raise SystemExit(1)

    counts, duplicate_dead, unmatched_lines = count_deaths(path, verbose=args.verbose)

    blue = counts.get("BLUE", 0)
    red  = counts.get("RED", 0)
    total = blue + red

    print("\n=== Death Count Summary ===")
    print(f"BLUE            : {blue}")
    print(f"RED             : {red}")
    print(f"TOTAL           : {total}")
    print(f"DUPLICATE DEAD  : {duplicate_dead}")

    if args.verbose and unmatched_lines:
        print("\n# Unmatched lines (contain 'dead' but couldn't infer BLUE/RED):")
        for ln, txt in unmatched_lines[:200]:
            print(f"[line {ln}] {txt}")
        if len(unmatched_lines) > 200:
            print(f"... and {len(unmatched_lines)-200} more")

if __name__ == "__main__":
    main()
