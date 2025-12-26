import argparse
from pathlib import Path

import pandas as pd


DEFAULT_OUTPUT = "data/result_stats_summary.csv"

EXCLUDE_COLUMNS = {
    "expIndex",
    "seed",
    "goal1Score",
    "goal2Score",
    "goal3Score",
    "totalScore",
}


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description="Print avg, min, max for numeric columns in result.csv"
    )
    parser.add_argument(
        "csv_path",
        nargs="?",
        default="data/result.csv",
        help="Path to result.csv (default: data/result.csv)",
    )
    parser.add_argument(
        "-o",
        "--output",
        default=DEFAULT_OUTPUT,
        help=f"Where to write the summary CSV (default: {DEFAULT_OUTPUT})",
    )
    return parser


def load_dataframe(csv_path: Path) -> pd.DataFrame:
    if not csv_path.exists():
        raise FileNotFoundError(f"CSV not found: {csv_path}")
    df = pd.read_csv(csv_path)
    numeric_df = df.drop(columns=[c for c in df.columns if c in EXCLUDE_COLUMNS], errors="ignore")
    numeric_df = numeric_df.select_dtypes(include="number")
    if numeric_df.empty:
        raise ValueError("No numeric columns found in CSV.")
    return numeric_df


def main() -> None:
    parser = build_parser()
    args = parser.parse_args()
    csv_path = Path(args.csv_path)
    output_path = Path(args.output)

    numeric_df = load_dataframe(csv_path)

    summary = numeric_df.agg(["mean", "min", "max"]).T
    summary = summary.rename(columns={"mean": "avg"})

    output_path.parent.mkdir(parents=True, exist_ok=True)
    summary.to_csv(output_path)

    # Print nicely aligned output for quick inspection.
    print(summary.to_string(float_format=lambda x: f"{x:,.3f}"))
    print(f"\nSummary saved to: {output_path}")


if __name__ == "__main__":
    main()
