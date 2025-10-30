#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
average.py
Parse multiple "=== Simulation Result ===" blocks from a text file (or stdin)
and print robust representative statistics suitable for stochastic wargame runs.

Now parses Objective Areas 1, 2, 3 for BOTH BLUE and RED.
Usage:
  python average.py --input runs.txt
  cat runs.txt | python average.py
  python average.py --trim 0.1 --verbose
"""

import sys
import re
import argparse
from dataclasses import dataclass, asdict
from typing import List, Dict, Any, Optional
import statistics as stats

@dataclass
class RunRow:
    seed: int
    # Objective Areas (BLUE/RED inside counts)
    obj1_blue_inside: int
    obj1_red_inside: int
    obj2_blue_inside: int
    obj2_red_inside: int
    obj3_blue_inside: int
    obj3_red_inside: int
    # Survivals / casualties
    blue_alive: int
    red_alive: int
    blue_casualties: int
    red_casualties: int
    blue_win: int  # 1 if red_alive == 0 else 0

OBJ_PATTERNS = {
    1: re.compile(r"Objective Area 1.*?BLUE inside\s*=\s*(\d+)\s*RED inside\s*=\s*(\d+)", re.S),
    2: re.compile(r"Objective Area 2.*?BLUE inside\s*=\s*(\d+)\s*RED inside\s*=\s*(\d+)", re.S),
    3: re.compile(r"Objective Area 3.*?BLUE inside\s*=\s*(\d+)\s*RED inside\s*=\s*(\d+)", re.S),
}

def parse_blocks(text: str) -> List[RunRow]:
    blocks = [b.strip() for b in text.split("=== Simulation Result ===") if b.strip()]
    rows: List[RunRow] = []
    for b in blocks:
        seed_m = re.search(r"Seed:\s*([-\d]+)", b)
        alive = re.search(r"Alive\s*-\s*BLUE:\s*(\d+)\s*/\s*RED:\s*(\d+)", b)
        cas = re.search(r"Casualties\s*-\s*BLUE:\s*(\d+)\s*/\s*RED:\s*(\d+)", b)
        # Objective Areas (1..3, BLUE/RED)
        obj_vals = {}
        for i in (1,2,3):
            m = OBJ_PATTERNS[i].search(b)
            if not m:
                # If a block lacks any objective line, skip this block
                obj_vals[i] = (0, 0)  # default to 0/0 rather than skip
            else:
                blue_in = int(m.group(1))
                red_in  = int(m.group(2))
                obj_vals[i] = (blue_in, red_in)

        if not (seed_m and alive and cas):
            # Skip malformed block
            continue

        seed = int(seed_m.group(1))
        blue_alive, red_alive = int(alive.group(1)), int(alive.group(2))
        blue_cas, red_cas = int(cas.group(1)), int(cas.group(2))

        rows.append(RunRow(
            seed=seed,
            obj1_blue_inside=obj_vals[1][0],
            obj1_red_inside=obj_vals[1][1],
            obj2_blue_inside=obj_vals[2][0],
            obj2_red_inside=obj_vals[2][1],
            obj3_blue_inside=obj_vals[3][0],
            obj3_red_inside=obj_vals[3][1],
            blue_alive=blue_alive,
            red_alive=red_alive,
            blue_casualties=blue_cas,
            red_casualties=red_cas,
            blue_win=int(red_alive == 0),
        ))
    # Sort by seed for stable display
    rows.sort(key=lambda r: r.seed)
    return rows

def trimmed_mean(values: List[float], proportion_to_cut: float) -> float:
    if not values:
        return float('nan')
    n = len(values)
    k = int(n * proportion_to_cut)
    if 2*k >= n:
        # Not enough data to trim; fall back to mean
        return sum(values) / n
    xs = sorted(values)[k:n-k]
    return sum(xs) / len(xs) if xs else float('nan')

def quantile(values: List[float], q: float) -> float:
    """Simple linear interpolation quantile (compatible across Python versions)."""
    if not values:
        return float('nan')
    xs = sorted(values)
    pos = (len(xs) - 1) * q
    lo = int(pos)
    hi = min(lo + 1, len(xs) - 1)
    frac = pos - lo
    return xs[lo] * (1 - frac) + xs[hi] * frac

def summarize(values: List[float], trim: float) -> Dict[str, float]:
    if not values:
        return {"mean": float('nan'), "median": float('nan'), "trimmed_mean": float('nan'),
                "std": float('nan'), "p25": float('nan'), "p75": float('nan'),
                "min": float('nan'), "max": float('nan')}
    mean = sum(values) / len(values)
    median = quantile(values, 0.5)
    tm = trimmed_mean(values, trim)
    # Sample std (ddof=1) when possible
    if len(values) > 1:
        m = mean
        var = sum((x - m) ** 2 for x in values) / (len(values) - 1)
        std = var ** 0.5
    else:
        std = float('nan')
    return {
        "mean": mean,
        "median": median,
        "trimmed_mean": tm,
        "std": std,
        "p25": quantile(values, 0.25),
        "p75": quantile(values, 0.75),
        "min": min(values),
        "max": max(values),
    }

def fmt_float(x: float) -> str:
    if x != x:  # NaN
        return "nan"
    # Smart formatting: integers shown as int, otherwise 2 decimals
    if abs(x - round(x)) < 1e-9:
        return f"{int(round(x))}"
    return f"{x:.2f}"

def print_table(title: str, rows: List[Dict[str, Any]], cols: List[str]) -> None:
    print(f"\n{title}")
    widths = {c: max(len(c), *(len(str(r.get(c, ''))) for r in rows)) for c in cols}
    header = " | ".join(c.ljust(widths[c]) for c in cols)
    sep = "-+-".join("-" * widths[c] for c in cols)
    print(header)
    print(sep)
    for r in rows:
        line = " | ".join(str(r.get(c, "")).ljust(widths[c]) for c in cols)
        print(line)

def main():
    ap = argparse.ArgumentParser(description="Compute robust summaries for TDGsim results (BLUE/RED Obj1-3).")
    ap.add_argument("--input", "-i", type=str, default="-", help="Input file path (default: stdin)")
    ap.add_argument("--trim", type=float, default=0.10, help="Trim proportion for trimmed mean (default: 0.10)")
    ap.add_argument("--verbose", action="store_true", help="Print per-run compact table")
    args = ap.parse_args()

    if args.input == "-" or args.input is None:
        text = sys.stdin.read()
    else:
        with open(args.input, "r", encoding="utf-8") as f:
            text = f.read()

    rows = parse_blocks(text)
    if not rows:
        print("No valid simulation blocks found.", file=sys.stderr)
        sys.exit(1)

    # Per-run compact view
    if args.verbose:
        compact = [{
            "seed": r.seed,
            "blue_alive": r.blue_alive,
            "red_alive": r.red_alive,
            "blue_cas": r.blue_casualties,
            "red_cas": r.red_casualties,
            "obj1_B": r.obj1_blue_inside,
            "obj1_R": r.obj1_red_inside,
            "obj2_B": r.obj2_blue_inside,
            "obj2_R": r.obj2_red_inside,
            "obj3_B": r.obj3_blue_inside,
            "obj3_R": r.obj3_red_inside,
            "blue_win": r.blue_win,
        } for r in rows]
        print_table("Runs (sorted by seed)", compact,
                    ["seed","blue_alive","red_alive","blue_cas","red_cas",
                     "obj1_B","obj1_R","obj2_B","obj2_R","obj3_B","obj3_R","blue_win"])

    # Build metric arrays
    blue_alive = [r.blue_alive for r in rows]
    red_alive = [r.red_alive for r in rows]
    blue_cas = [r.blue_casualties for r in rows]
    red_cas = [r.red_casualties for r in rows]

    # Objective arrays
    obj_metrics = {
        "obj1_blue_inside": [r.obj1_blue_inside for r in rows],
        "obj1_red_inside":  [r.obj1_red_inside  for r in rows],
        "obj2_blue_inside": [r.obj2_blue_inside for r in rows],
        "obj2_red_inside":  [r.obj2_red_inside  for r in rows],
        "obj3_blue_inside": [r.obj3_blue_inside for r in rows],
        "obj3_red_inside":  [r.obj3_red_inside  for r in rows],
    }

    blue_win_rate = sum(r.blue_win for r in rows) / len(rows)

    # Occupancy derived stats
    def occ_stats(arr: List[int]):
        n = len(arr)
        occur = sum(1 for x in arr if x > 0)
        rate = occur / n
        cond_vals = [x for x in arr if x > 0]
        cond_mean = (sum(cond_vals)/len(cond_vals)) if cond_vals else 0.0
        return rate, cond_mean

    # Summary tables for survivals/casualties
    basic_metrics = {
        "blue_alive": blue_alive,
        "red_alive": red_alive,
        "blue_casualties": blue_cas,
        "red_casualties": red_cas,
    }

    rows_out = []
    for name, vals in {**basic_metrics, **obj_metrics}.items():
        s = summarize([float(v) for v in vals], args.trim)
        rows_out.append({
            "metric": name,
            "median": fmt_float(s["median"]),
            "IQR[p25,p75]": f"[{fmt_float(s['p25'])}, {fmt_float(s['p75'])}]",
            f"trimmed_mean({int(args.trim*100)}%)": fmt_float(s["trimmed_mean"]),
            "mean": fmt_float(s["mean"]),
            "std": fmt_float(s["std"]),
            "min": fmt_float(s["min"]),
            "max": fmt_float(s["max"]),
        })

    print_table("Summary (robust)", rows_out,
                ["metric","median","IQR[p25,p75]",f"trimmed_mean({int(args.trim*100)}%)","mean","std","min","max"])

    # KPI block
    print("\nKPI")
    print(f"- BLUE win rate                   : {blue_win_rate*100:.1f}%")
    for i in (1,2,3):
        for side in ("blue","red"):
            arr = obj_metrics[f"obj{i}_{side}_inside"]
            rate, cond_mean = occ_stats(arr)
            label = f"Obj{i} {side.upper()} inside"
            print(f"- {label:26s} : occurrence {rate*100:5.1f}% | conditional mean {cond_mean:.2f}")

if __name__ == "__main__":
    main()
