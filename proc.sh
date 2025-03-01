echo Start compilling
cd processor/src
g++ -Wall -c ProcFunc.cpp -c ProcMain.cpp -c StackFunc.cpp
g++ -Wall -o proc ProcFunc.o ProcMain.o StackFunc.o
echo End compilling
cd ../..
sleep .5
rm processor/src/*.o
mv ./processor/src/proc ./
