brainfuck: src/main.c src/brainfuck.c src/brainfuck.h tests/tests.c
	$(CC) -o src/main.o -c src/main.c
	$(CC) -o src/brainfuck.o -c src/brainfuck.c
	$(CC) -o brainfuck src/brainfuck.o src/main.o

all: brainfuck

tests: brainfuck
	$(CC) -o brainfuck_tests -Isrc tests/tests.c src/brainfuck.o
	./brainfuck_tests
