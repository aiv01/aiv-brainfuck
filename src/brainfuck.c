#include "brainfuck.h"

int brainfuck_init(struct brainfuck_vm *vm, unsigned int memory_size)
{
	// clear the memory of the struct
	memset(vm, 0, sizeof(struct brainfuck_vm));
	// save the size of the memory for safety checks
	vm->memory_size = memory_size;
	// allocate a new memory for the vm
	vm->memory = (char *)malloc(memory_size);
	if (!vm->memory)
		return -1;
	// zero all the memory
	memset(vm->memory, 0, memory_size);
	vm->ptr = vm->memory;
	return 0;
}

void brainfuck_reset(struct brainfuck_vm *vm)
{
	vm->code = NULL;
	vm->code_size = 0;
	vm->pc = 0;
	vm->ptr = vm->memory;
}

void brainfuck_destroy(struct brainfuck_vm *vm)
{
	free(vm->memory);
}

static int brainfuck_opcode_run(struct brainfuck_vm *vm)
{
	// retrieve the current opcode from the code/text area
	char opcode = vm->code[vm->pc];

	switch (opcode) {
		case '>':
			if (vm->ptr < vm->memory + vm->memory_size - 1) {
				vm->ptr++;
				break;
			}
			break;
		case '<':
			if (vm->ptr > vm->memory) {
				vm->ptr--;
				break;
			}
			break;
		case '+':
			(*vm->ptr)++;
			break;
		case '-':
			(*vm->ptr)--;
			break;
		case '.':
			if (vm->print_hook) {
				vm->print_hook(vm);
			}
			else {
				fputc(*vm->ptr, stdout);
			}
			break;
		case ',':
			if (vm->input_hook) {
				*vm->ptr = vm->input_hook(vm);
			}
			else {
				*vm->ptr = fgetc(stdin);
			}
			break;
		case '[':
			if (*vm->ptr == 0) {
				unsigned int to_find = 1;
				unsigned int pc_scanner = vm->pc + 1;
				// search for closing bracket, take into account nested ones
				while(pc_scanner < vm->code_size) {
					if (vm->code[pc_scanner] == '[') {
						to_find++;
					}
					else if (vm->code[pc_scanner] == ']') {
						// something is seriously wrong
						if (to_find == 0)
							return BF_ERR;
						to_find--;
						if (to_find == 0)
							return ++pc_scanner;
					}
					pc_scanner++;
				}
				return BF_ERR;
			}
			break;
		case ']':
			if (*vm->ptr != 0) {
                                unsigned int to_find = 1;
				// something is seriously wrong
				if (vm->pc == 0)
					return BF_ERR;
                                unsigned int pc_scanner = vm->pc - 1;
                                // search for previous opening bracket, take into account nested ones
                                while(pc_scanner > 0) {
                                        if (vm->code[pc_scanner] == ']') {
                                                to_find++;
                                        }
                                        else if (vm->code[pc_scanner] == '[') {
						// something is seriously wrong
						if (to_find == 0)
							return BF_ERR;
                                                to_find--;
                                                if (to_find == 0)
                                                        return ++pc_scanner;
                                        }
                                        pc_scanner--;
                                }
                                return BF_ERR;
                        }
			break;
		default:
			// comment
			break;
	}
	return vm->pc + 1;
}

int brainfuck_run(struct brainfuck_vm *vm, char *code, unsigned int code_size, unsigned int *opcode_error_pos)
{
	vm->code = code;
	vm->code_size = code_size;
	vm->pc = 0;

	for(;;) {
		int ret = brainfuck_opcode_run(vm);
		if (ret >= 0) {
			if (ret >= vm->code_size) {
				// end of program
				return 0;
			}
			vm->pc = ret;
			continue;
		}
		break;
	}

	if (opcode_error_pos)
		*opcode_error_pos = vm->pc;
	return -1;
}
