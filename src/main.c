#include "brainfuck.h"

int main(int argc, char *argv[])
{
	int ret = 0;

	if (argc < 2) {
		fprintf(stderr, "plase specify the brainfuck vm memory size\n");
		return -1;
	}

	struct brainfuck_vm vm;
	if (brainfuck_init(&vm, atoi(argv[1]))) {
		fprintf(stderr, "unable to initialize brainfuck vm memory\n");
		return -1;
	}

	fprintf(stdout, "AIV brainfuck VM initialized with %u bytes of memory\n", vm.memory_size);

	if (argc > 2) {
		FILE *source = fopen(argv[2], "r");
		if (!source) {
			fprintf(stderr, "unable to open file\n");
			goto end;
		}

		fseek(source, 0, SEEK_END);
		long rlen = ftell(source);
		fseek(source, 0, SEEK_SET);

		char *code = (char *)malloc(rlen);
		if (!code) {
			fprintf(stderr, "unable to allocate code memory\n");
			goto end;
		}

		fread(code, 1, rlen, source);

		unsigned int opcode_err_pos = 0;

		ret = brainfuck_run(&vm, code, rlen, &opcode_err_pos);
                if (ret < 0) {
                	fprintf(stderr, "error while running opcode '%c' at position %u\n", code[opcode_err_pos], opcode_err_pos);
                }
		// free resources
                free(code);
		fclose(source);
	} else {
		unsigned int line_number = 0;
		for (;;) {
			char line_buf[4096];
			char *line = fgets(line_buf, 4096, stdin);
			if (!line)
				break;
			unsigned int opcode_err_pos = 0;
			ret = brainfuck_run(&vm, line, strlen(line), &opcode_err_pos);
			if (ret < 0) {
				fprintf(stderr, "error while running opcode '%c' at position %u line: %u \n", line[opcode_err_pos], opcode_err_pos, line_number);
				break;
			}
			line_number++;
		}
	}

end:
	brainfuck_destroy(&vm);

	return ret;
}
