CFLAGS=-g -fsanitize=address
FLAGS=-Wall -Wextra -Werror -O2 -std=c99 -pedantic
CC=cc
PORT=8080

MAIN=./src/main.c
FILES=./src/utils.c ./src/file/files.c ./src/client/client.c

all: build

run: debug
	@echo "the app is started"
	@./bin/main.out $(PORT)

debug:
	$(CC) $(CFLAGS) $(MAIN) $(FILES) -o ./bin/main.out $(FLAGS)

build:
	$(CC) $(MAIN) $(FILES) -o ./bin/release.out

clean:
	@rm -rf ./bin/*
	@echo "it is clean now"
