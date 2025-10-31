import re
import argparse
from pathlib import Path
from statistics import mean, median
import matplotlib.pyplot as plt


def extract_scores(path: Path):
    pattern = re.compile(r"^Total Score:\s*([0-9]*\.?[0-9]+)")
    scores = []
    with path.open("r", encoding="utf-8", errors="ignore") as f:
        for line in f:
            m = pattern.search(line)
            if m:
                try:
                    scores.append(float(m.group(1)))
                except ValueError:
                    pass
    return scores


def main():
    parser = argparse.ArgumentParser(description="Extract and plot distribution of Total Score from result_summary.txt")
    parser.add_argument("--file", default="result_summary.txt", type=Path, help="Path to result_summary.txt")
    parser.add_argument("--out", default="score_hist.png", type=Path, help="Output image file for histogram")
    parser.add_argument("--bins", default=20, type=int, help="Number of histogram bins")
    parser.add_argument("--show", action="store_true", help="Show the plot window")
    args = parser.parse_args()

    if not args.file.exists():
        raise SystemExit(f"File not found: {args.file}")

    scores = extract_scores(args.file)
    if not scores:
        raise SystemExit("No 'Total Score' entries found.")

    print(f"Count: {len(scores)}")
    print(f"Min:   {min(scores):.3f}")
    print(f"Max:   {max(scores):.3f}")
    print(f"Mean:  {mean(scores):.3f}")
    print(f"Median:{median(scores):.3f}")

    plt.figure(figsize=(7, 4.5))
    plt.hist(scores, bins=args.bins, color="#4C78A8", edgecolor="white")
    plt.title("Total Score Distribution")
    plt.xlabel("Total Score")
    plt.ylabel("Frequency")
    plt.grid(axis="y", alpha=0.25, linewidth=0.7)
    plt.tight_layout()

    args.out.parent.mkdir(parents=True, exist_ok=True)
    plt.savefig(args.out, dpi=150)
    print(f"Saved histogram to: {args.out}")

    if args.show:
        plt.show()


if __name__ == "__main__":
    main()

