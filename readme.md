## Windows

### 실행
g++ -std=gnu++17 -O2 -I. \
  $(find . -name "*.cpp") \
  -static-libstdc++ -static-libgcc \
  -o tdg.exe && ./tdg.exe

### 실험
.\run_exp.ps1 -Runs <experiment_number>

<experiment_number>만큼 시뮬레이션을 반복 실행, 스크립트 실행 시 CSV 및 로그 파일 초기화

### 시각화
python visualizer.py --log <log_number>

log_simiulation_exp<log_number>.txt(앞자리 0제외) 로그를 선택하여 visualize