# aiv-brainfuck
C implementation of the Brainfuck language for AIV third year programming course

For Linux, Windows and Mac

## Usage

./brainfuck[.exe] <memory_size> [filename]

if 'filename' is specified the brainfuck interpreter will parse the specified file, otherwise it will
wait for commands in the stdin

## Bulding

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
