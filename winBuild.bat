@echo off
if not exist build mkdir build
cd build
cmake -S ../ -B .
make && make Shaders && ./LearnVK
cd ../