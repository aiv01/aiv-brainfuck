# aiv-brainfuck
C implementation of the Brainfuck language for AIV third year programming course

For Linux, Windows and Mac

## Usage

./brainfuck[.exe] <memory_size> [filename]

if 'filename' is specified the brainfuck interpreter will parse the specified file, otherwise it will
wait for commands in the stdin

## Bulding

LLVM/clang, gcc, cl.exe are supported.

On POSIX systems:

```sh
make
```

On Windows systems (from the Visual Studio console)

```sh
nmake -f Makefile.nmake
```

## Tests

To run the unit tests, execute the 'tests' make target:

```sh
make tests
```

or (for Windows)

```sh
nmake -f Makefile.nmake tests
```

## Embedding API

the brainfuck_vm structure represents a brainfuck virtual machine.

The following functions are exposed:

```c
int brainfuck_init(struct brainfuck_vm *, unsigned int);
```

must be called before working with a vm, initialize a new virtualmachine with specified memory size. Returns an int != 0 on error.

```c
void brainfuck_destroy(struct brainfuck_vm *);
```

destroy a brainfuck virtual machine

```c
void brainfuck_reset(struct brainfuck_vm *);
```

reset a brainfuck virtual machine

```c
int brainfuck_run(struct brainfuck_vm *, char *, unsigned int, unsigned int *);
```

executes a string of opcodes with the specified size. The last argument (if not NULL) is a pointer to the position of the opcode that triggered an error. A return value != 0, means error and the VM should be reset or destroyed by the user.

## Input and Output

The VM can be configured for custom input and output hooks. If no hooks are defined, the '.' command will call fputf() while ',' will call fgetc().

## Example: printing CIAO

```
+++++++++++++
[
        >
        +++++
        <-
]
>++.
++++++.
--------.
++++++++++++++.
```
