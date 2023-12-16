CC=g++
CFLAGS=-std=c++11

SRC=main.cpp
DST=build/main.o

LDS=-ldl -lSDL2

TEST=test.cpp
TDST=test/test.o

INC_GRID = grid
INC_POINT = point
INCS = -I$(INC_GRID) -I$(INC_POINT)

SRC_GRID = grid/grid.cpp
SRC_POINT = point/point.cpp
SRCS = $(SRC_GRID) $(SRC_POINT)

run:
	./$(DST)

clean:
	rm -rf build
	rm -rf test

build: main.cpp
	rm -rf build
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(DST) $(LDS)

debug: main.cpp
	rm -rf build
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -g -o $(DST) $(LDS)


test: $(TEST)
	rm -rf test
	mkdir -p test
	$(CC) $(CFLAGS) $(TEST) $(SRCS) -o $(TDST) $(INCS) $(LDS)

