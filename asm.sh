echo Start compilling
cd assembler/src
g++ -Wall -c AsmFunc.cpp -c AsmMain.cpp
g++ -Wall -o asm AsmMain.o AsmFunc.o
echo End compilling
cd ../..
sleep .5
rm assembler/src/*.o
mv ./assembler/src/asm ./
