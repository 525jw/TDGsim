## Windows
### 컴파일&&실행
g++ -std=gnu++17 -O2 -I. \
  $(find . -name "*.cpp") \
  -static-libstdc++ -static-libgcc \
  -o tdg.exe && ./tdg.exe
### 실험
.\run_exp.ps1 -Runs <experiment_number>

## Linux
### 컴파일&&실행
g++ -std=gnu++17 -O2 -I. \
  $(find . -name "*.cpp") \
  -o tdg && ./tdg
### 실험
./run_exp.sh -r <experiment_number>

## 시각화
python visualizer.py --log <log_number> --snapshot <snapshot_number>
python visualizer.py --log 3 --snapshot 3 --cell 8