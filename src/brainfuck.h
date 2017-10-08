#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define BF_OK	0
#define BF_ERR	-1

struct brainfuck_vm {
	char *code;
	unsigned int code_size;
	// program counter
	unsigned int pc;
	char *memory;
	unsigned int memory_size;
	char *ptr;
	void (*print_hook)(struct brainfuck_vm *);
	char (*input_hook)(struct brainfuck_vm *);
	int (*pre_hook)(struct brainfuck_vm *);
	int (*post_hook)(struct brainfuck_vm *);
};

int brainfuck_init(struct brainfuck_vm *, unsigned int);
void brainfuck_destroy(struct brainfuck_vm *);

void brainfuck_reset(struct brainfuck_vm *);

int brainfuck_run(struct brainfuck_vm *, char *, unsigned int, unsigned int *);
