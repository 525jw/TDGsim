import pandas as pd
import matplotlib.pyplot as plt
from pathlib import Path
import re
import sys

DATA_DIR = Path(__file__).resolve().parent

files = list(DATA_DIR.glob("*.csv"))
print("[DEBUG] DATA_DIR:", DATA_DIR)
print("[DEBUG] csv files:", [f.name for f in files])

if not files:
    print("[ERROR] 같은 폴더에 csv가 없음. plot.py랑 csv를 같은 폴더에 둬야 함.")
    sys.exit(1)

def run_number(p: Path):
    m = re.findall(r"\d+", p.stem)
    return int(m[0]) if m else 10**9

files = sorted(files, key=run_number)

METRICS = ["blueCasualties", "redCasualties", "goal1Blue", "goal2Blue", "goal3Blue"]

runs = []
skipped = []

for f in files:
    try:
        df = pd.read_csv(f)
        print(f"\n[DEBUG] reading {f.name} columns={df.columns.tolist()} rows={len(df)}")

        # metric column 처리
        if "Unnamed: 0" in df.columns:
            df = df.rename(columns={"Unnamed: 0": "metric"})
        elif "metric" not in df.columns:
            skipped.append((f.name, "no metric/Unnamed: 0 col"))
            continue

        # avg/min/max 컬럼 확인
        need_cols = {"avg", "min", "max"}
        if not need_cols.issubset(df.columns):
            skipped.append((f.name, f"missing cols {need_cols - set(df.columns)}"))
            continue

        df = df.set_index("metric")[["avg", "min", "max"]]

        for c in ["avg", "min", "max"]:
            df[c] = pd.to_numeric(df[c], errors="coerce")

        run_id = run_number(f)
        df["run"] = run_id
        runs.append(df.reset_index())

    except Exception as e:
        skipped.append((f.name, f"exception: {e}"))

print("\n[DEBUG] runs built:", len(runs))
print("[DEBUG] skipped:", skipped)

if not runs:
    print("\n[ERROR] 모든 csv가 스킵되어 runs가 비었음.")
    print("위 skipped 이유를 보고 csv 포맷(컬럼명)을 맞춰야 함.")
    sys.exit(1)

all_df = pd.concat(runs, ignore_index=True)
all_df = all_df[all_df["metric"].isin(METRICS)].sort_values(["metric", "run"])
x = sorted(all_df["run"].unique())

# ---- plot ----
fig, axes = plt.subplots(len(METRICS), 1, figsize=(10, 2.6 * len(METRICS)), sharex=True)
if len(METRICS) == 1:
    axes = [axes]

for ax, m in zip(axes, METRICS):
    sub = all_df[all_df["metric"] == m].sort_values("run")
    xr  = sub["run"].to_numpy(dtype=float)
    avg = sub["avg"].to_numpy(dtype=float)
    mn  = sub["min"].to_numpy(dtype=float)
    mx  = sub["max"].to_numpy(dtype=float)

    ax.plot(xr, avg, marker="o", label="avg")
    ax.fill_between(xr, mn, mx, alpha=0.2, label="min~max band")
    ax.vlines(xr, mn, mx, linewidth=1, alpha=0.6)

    ax.set_title(m)
    ax.set_ylabel("value")
    ax.grid(True, alpha=0.3)
    ax.legend(loc="best", fontsize=9)

axes[-1].set_xticks(x)
axes[-1].set_xlabel("run (file number)")
plt.tight_layout()
plt.show()
