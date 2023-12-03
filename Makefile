CC=g++
CFLAGS=-std=c++11

SRC=main.cpp
DST=build/main.o

INC=-ldl -lSDL2

build: $(DST)

run:
	./$(DST)

clean:
	rm -rf build

$(DST): main.cpp
	mkdir -p build
	$(CC) $(CFLAGS) $(SRC) -o $(DST) $(INC)


