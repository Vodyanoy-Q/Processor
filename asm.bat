@echo off
cd assembler/src
g++ -Wall -c AsmFunc.cpp -c AsmMain.cpp
g++ -Wall -o asm AsmMain.o AsmFunc.o
cd ..
cd ..
move "assembler\src\asm.exe" ".\" >nul 2>&1
