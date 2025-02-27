CC=g++
CFLAGS=-Wall -g -std=c++17
LIBS=-lmosquitto
TARGET=simulator

.PHONY: all clean

all:
	$(CC) main.cpp $(CFLAGS) $(LIBS) -o $(TARGET)

clean:
	$(RM) *.o $(TARGET)
