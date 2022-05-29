main:main.o CRC32.o #生成main 需要用到编译好的shingle.o
	g++ main.o CRC32.o -o main
CRC32.o:CRC32.cpp
	g++ -c CRC32.cpp
shingle.o:main.cpp
	g++ -c main.cpp