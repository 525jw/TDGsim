g++ -std=gnu++17 -O2 -I. \
  $(find . -name "*.cpp") \
  -static-libstdc++ -static-libgcc \
  -o tdg.exe && ./tdg.exe


.\run_exp.ps1 -Runs <experiment_number>
