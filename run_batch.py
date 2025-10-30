import subprocess
import sys
from pathlib import Path


def compile_project():
    root = Path(__file__).parent
    logs_dir = root / "logs"
    out_dir = root / "out"
    logs_dir.mkdir(parents=True, exist_ok=True)
    out_dir.mkdir(parents=True, exist_ok=True)

    # Collect sources
    cpp_dirs = [root / "DEVS", root / "EF", root / "SIM"]
    cpp_files = [str(root / "main.cpp")]
    for d in cpp_dirs:
        if d.exists():
            for p in d.rglob("*.cpp"):
                cpp_files.append(str(p))

    # Build command
    cmd = [
        "g++",
        "-std=gnu++17",
        "-O2",
        "-I.",
        *cpp_files,
        "-static-libstdc++",
        "-static-libgcc",
        "-o",
        "tdg.exe",
    ]

    print("[Build] Running:", " ".join(cmd))
    try:
        subprocess.run(cmd, check=True, cwd=root)
    except FileNotFoundError:
        print("Error: g++ not found in PATH.")
        sys.exit(1)
    except subprocess.CalledProcessError as e:
        print(f"Build failed with code {e.returncode}.")
        sys.exit(e.returncode)

    if not (root / "tdg.exe").exists():
        print("Build did not produce tdg.exe")
        sys.exit(1)
    print("[Build] Success: tdg.exe ready.")


def run_and_archive(runs: int = 100):
    root = Path(__file__).parent
    exe = root / "tdg.exe"
    logs_dir = root / "logs"
    logs_dir.mkdir(exist_ok=True)

    for i in range(1, runs + 1):
        print(f"=== Run {i} start ===")

        # Clean fixed-name logs to avoid stale copies
        for candidate in (root / "log_simulation.txt", logs_dir / "log_simulation.txt"):
            if candidate.exists():
                try:
                    candidate.unlink()
                except Exception:
                    pass

        # Run simulator (adjust args as needed)
        cmd = [str(exe), "--map", "map.json"]
        result = subprocess.run(cmd, cwd=root)
        if result.returncode != 0:
            print(f"Warning: tdg.exe exited with code {result.returncode} on run {i}")

        # Locate produced log and copy to unique path
        src = None
        if (root / "log_simulation.txt").exists():
            src = root / "log_simulation.txt"
        elif (logs_dir / "log_simulation.txt").exists():
            src = logs_dir / "log_simulation.txt"

        if src is not None:
            dst = logs_dir / f"log_simulation_{i}.txt"
            try:
                dst.write_bytes(src.read_bytes())
                print(f"Saved -> {dst}")
            except Exception as e:
                print(f"Error copying log for run {i}: {e}")
        else:
            print(f"Warning: No log_simulation.txt found after run {i}")

        print(f"=== Run {i} end ===\n")


def main():
    compile_project()
    run_and_archive(100)


if __name__ == "__main__":
    main()

