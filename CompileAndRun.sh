echo "Compiling the inspect program";
g++ src/inspection_console.cpp ./src/Myutilities.cpp -lncurses -lm -o bin/inspection
echo "Compiling the cmd program";
g++ src/command_console.cpp ./src/Myutilities.cpp -lncurses -o bin/command
echo "Compiling the RW program";
g++ -o ./bin/RW ./src/RW.cpp ./src/Myutilities.cpp
echo "Compiling the M1 program";
g++ -o ./bin/M1 ./src/M1.cpp ./src/Myutilities.cpp
echo "Compiling the M2 program";
g++ -o ./bin/M2 ./src/M2.cpp ./src/Myutilities.cpp
echo "Compiling the main program";
g++ src/master.cpp ./src/Myutilities.cpp -o bin/master
echo "Runing the main program ";
./bin/master