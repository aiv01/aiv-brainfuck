#include <brainfuck.h>

static unsigned int tests_counter = 0;

static struct brainfuck_vm vm;

static char test_print_data[64];
static unsigned int test_print_cursor = 0;

static void test_print(struct brainfuck_vm *vm)
{
	test_print_data[test_print_cursor++] = *vm->ptr;	
}

static char test_input(struct brainfuck_vm *vm)
{
	return 'x';
}

static void clean_test_print_data()
{
	memset(test_print_data, 0, 64);
	test_print_cursor = 0;
}

static void assert_zero(int value)
{
	if (value != 0) {
		fprintf(stderr, "[test %u \"%.*s\" *ptr=%d] %d is not zero\n", tests_counter, vm.code_size, vm.code, value, *vm.ptr);		
		exit(1);
	}
	tests_counter++;
}

static void assert_non_zero(int value)
{
	if (value == 0) {
		fprintf(stderr, "[test %u \"%.*s\" *ptr=%d] %d is zero\n", tests_counter, vm.code_size, vm.code, value, *vm.ptr);		
		exit(1);
	}
	tests_counter++;
}

static void assert_is_equal(int value, int cmp)
{
	if (value != cmp) {
		fprintf(stderr, "[test %u \"%.*s\" *ptr=%d] %d is not %d\n", tests_counter, vm.code_size, vm.code, value, cmp, *vm.ptr);
		exit(1);
	}
	tests_counter++;
}

static void assert_is_not_equal(int value, int cmp)
{
	if (value == cmp) {
		fprintf(stderr, "[test %u \"%.*s\" *ptr=%d] %d is not %d\n", tests_counter, vm.code_size, vm.code, value, cmp, *vm.ptr);
		exit(1);
	}
	tests_counter++;
}

#define BF_RUN(x) brainfuck_run(&vm, x, sizeof(x)-1, NULL)

int main(int argc, char *argv[])
{

	clean_test_print_data();
	
	assert_zero(brainfuck_init(&vm, 1024));

	vm.print_hook = test_print;
	vm.input_hook = test_input;

	assert_is_equal(vm.memory_size, 1024);

	assert_is_equal(vm.memory[0], 0);

	assert_zero(BF_RUN("+++"));
	assert_is_equal(vm.memory[0], 3);

	assert_zero(BF_RUN("---"));
	assert_zero(vm.memory[0]);

	assert_zero(BF_RUN("><+++"));
	assert_is_equal(vm.memory[0], 3);

	assert_non_zero(BF_RUN("]]"));

	assert_zero(BF_RUN("---><+++[-]"));
	assert_zero(vm.memory[0]);

	assert_zero(BF_RUN("---><+++---[+++]"));
	assert_zero(vm.memory[0]);

	assert_zero(BF_RUN("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++."));

	assert_zero(strcmp(test_print_data, "Hello World!\n"));

	brainfuck_reset(&vm);

	assert_zero(BF_RUN(">>,"));
	assert_is_equal(vm.memory[2], (int)'x');

	fprintf(stdout, "OK [%u tests executed]\n", tests_counter);
}
