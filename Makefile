CFLAGS = -Wall -Wextra -g -std=c99 -lncurses
CC = gcc

all: main

main: src/main.c
	$(CC) $(CFLAGS) -o main src/main.c
