all:
	$(CC) -o brainfuck src/brainfuck.c src/main.c

test: brainfuck_tests
	$(CC) -o brainfuck_tests -Isrc tests/tests.c src/brainfuck.c
	./brainfuck_tests

sdl:
	$(CC) -o brainfuck_sdl src/brainfuck.c src/sdl.c -lSDL2
