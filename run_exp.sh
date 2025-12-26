set -euo pipefail

# 기본값
RUNS=1
SLEEP_SECONDS=0

# 인자 파싱
while [[ $# -gt 0 ]]; do
  case "$1" in
    -r|--runs)
      RUNS="$2"
      shift 2
      ;;
    -s|--sleep)
      SLEEP_SECONDS="$2"
      shift 2
      ;;
    *)
      echo "Unknown option: $1"
      exit 1
      ;;
  esac
done

# 스크립트 위치 기준으로 실행
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT"

# logs 정리
if [[ -d "logs" ]]; then
  rm -f logs/*
fi

# result.csv 초기화
if [[ -f "data/result.csv" ]]; then
  : > data/result.csv
fi

# 실행 루프
for ((i=1; i<=RUNS; i++)); do
  echo "Run $i/$RUNS"
  ./tdg
  EXIT_CODE=$?

  if [[ $EXIT_CODE -ne 0 ]]; then
    echo "tdg failed with exit code $EXIT_CODE"
    exit $EXIT_CODE
  fi

  if [[ $SLEEP_SECONDS -gt 0 ]]; then
    sleep "$SLEEP_SECONDS"
  fi
done
