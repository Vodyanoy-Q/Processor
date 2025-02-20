@echo off
cd processor/src
g++ -Wall -c ProcFunc.cpp -c ProcMain.cpp -c StackFunc.cpp
g++ -Wall -o proc ProcFunc.o ProcMain.o StackFunc.o
cd ..
cd ..
move "processor\src\proc.exe" ".\" >nul 2>&1
