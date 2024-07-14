CC=cc
FLAGS=-Wall

all: main

main:
	$(CC) ./src/main.c ./src/utils.c -o ./bin/main $(FLAGS)

run: main
	@echo "the app is started"
	@./bin/main

clean:
	@rm ./bin/*
	@echo "it is clean now"
