# aiv-brainfuck
C implementation of the Brainfuck language for AIV third year programming course

For Linux, Windows and Mac

## Usage

./brainfuck[.exe] <memory_size> [filename]

if 'filename' is specified the brainfuck interpreter will parse the specified file, otherwise it will
wait for commands in the stdin

Note that when in REPL mode (read: when no file is specified), the '[' and ']' commands cannot be split on multiple lines.

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

## Commands

`+` increment the value of the pointer

`-` decrement the value of the pointer

`>` move the pointer right

`<` move the pointer left

`[` start of a block, enter if the value of the pointer is non zero

`]` end of a block, exit if the value of the pointer is zero, otherwise back to the start of the block

`.` output (can be hooked) the current value of the pointer

`,` input (can be hooked) to the current pointer

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

## Hooks

the brainfuck_vm exposes the following hooks

```c
// called by the dot command
void (*print_hook)(struct brainfuck_vm *);
// called by the comma command
char (*input_hook)(struct brainfuck_vm *);
// called before each opcode (non zero return value triggers an error)
int (*pre_hook)(struct brainfuck_vm *);
// called after each opcode (non zero return value triggers an error)
int (*post_hook)(struct brainfuck_vm *);
```

## SDL Custom VM

As a proof of concept, an SDL based vm is included. You need to pass the width and the height of a window to the command line options.

The VM memory will be composed by the classic general-purpose brainfuck memory followed by a 8 bytes IVT (Interrupt Vector Table) and a width * height * 4 sequence of bytes representing the RGBA pixels. Yes you can write colors to the window using brainfuck :)

```sh
./brainfuck_sdl[.exe] 10 300 100 [file]
```

will generate a brainfuck vm with 10 bytes of general purpose memory, 8 bytes of IVT and 300 * 100 * 4 bytes of RGBA pixels.

The IVT table is a special memory area triggering specific behaviours (like a true interrupt table).

Currently 3 fields are defined:

* IVT_REDRAW (address 0 of the IVT) when set to 1, the content of the RGBA pixel area will be transferred to the window (synchronized with VSYNC), it is automatically reset to 0

* IVT_SLEEP (address 1 of the IVT), suspend the VM for the number of milliseconds specified in the IVT_SLEEP_VALUE field, it is automatically reset to 0

* IVT_SLEEP_FIELD (address 2 of the IVT), see IVT_SLEEP, note that the field is interpreted as unsigned char (0-255)

## Examples

### printing CIAO [without newline]

```bf
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

### Waiting for user input and print it inverted [with newline]

```bf
>+>+
[
        <
        [
                ,>[-]>[-]>[-]<<<
                [
                        >+>+>+
                        <<<-
                ]
                >----------
                [
                        >
                        [
                                <<+
                                >>-
                        ]
                ]
                >----------
                [<]
                <
        ]
        <<[.<]++++++++++.----------
        >+>+
]

```
