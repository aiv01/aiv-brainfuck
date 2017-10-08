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
			char *line = NULL;
			size_t len = 0;
			ssize_t rlen = getline(&line, &len, stdin);
			if (rlen < 0)
				break;
			unsigned int opcode_err_pos = 0;
			ret = brainfuck_run(&vm, line, rlen, &opcode_err_pos);
			if (ret < 0) {
				fprintf(stderr, "error while running opcode '%c' at position %u line: %u \n", line[opcode_err_pos], opcode_err_pos, line_number);
				break;
			}
			//free the memory allocated by getline
			free(line);
			line_number++;
		}
	}

end:
	brainfuck_destroy(&vm);

	return ret;
}
