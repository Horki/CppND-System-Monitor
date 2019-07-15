CC=g++
FLAGS=-std=c++17 -Wall -Werror -lncurses

BIN=hhtop

all: $(BIN)

hhtop: main.cpp constants.h Process.h ProcessParser.h ProcessContainer.h SysInfo.h util.h
	$(CC) $(FLAGS) -o $@ $<

clean:
	rm -If $(BIN)
