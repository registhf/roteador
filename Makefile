all: prog

prog: *.c *.h
	gcc *.c -pthread -o Router -Wall -g

clean:
	rm Router
