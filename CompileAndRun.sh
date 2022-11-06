
echo "Compiling the cmd program\r";
g++ -o ./bin/cmd ./src/cmd.cpp ./src/Myutilities.cpp
echo "Compiling the inspect program";
g++ -o ./bin/inspect ./src/inspect.cpp ./src/Myutilities.cpp
echo "Compiling the RW program";
g++ -o ./bin/RW ./src/RW.cpp ./src/Myutilities.cpp
echo "Compiling the M1 program";
g++ -o ./bin/M1 ./src/M1.cpp ./src/Myutilities.cpp
echo "Compiling the M2 program";
g++ -o ./bin/M2 ./src/M2.cpp ./src/Myutilities.cpp

echo "Compiling the main program";
g++ -o ./bin/main ./src/main.cpp ./src/Myutilities.cpp
echo "Runing the main program ";
#./bin/main