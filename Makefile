all: prog

prog: *.c *.h
	gcc *.c -pthread -lncurses -o Router -Wall -g

clean:
	rm Router
