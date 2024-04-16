all: main

CXXFLAGS = -std=c++20 -g -O0 -pedantic -Wall -lpthread

main: main.cpp
	clang++ ${CXXFLAGS} -o main main.cpp #-I.

clean:
	rm main
