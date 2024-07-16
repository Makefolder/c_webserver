CFLAGS=-g -fsanitize=address
FLAGS=-Wall -Wextra -Werror -O2 -std=c99 -pedantic
CC=cc
PORT=8080

MAIN=./src/main.c

all: build

run: debug
	@echo "the app is started"
	@./bin/main.out $(PORT)

debug:
	$(CC) $(CFLAGS) $(MAIN) ./src/utils.c -o ./bin/main.out $(FLAGS)

build:
	$(CC) $(MAIN) ./src/utils.c -o ./bin/release.out

clean:
	@rm -rf ./bin/*
	@echo "it is clean now"
