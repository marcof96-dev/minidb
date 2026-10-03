# minidb

minidb is a toy key-value store written in C.
It was implemented from scratch for study purposes. I followed [antirez's C course](https://www.youtube.com/playlist?list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY) for many weeks, and this project was born from the desire to put his teachings into practice.
The store is built on a hash table that handles insertion, update, deletion and retrieval of data; collisions are handled through a Separate Chaining algorithm.
It resizes itself automatically: once the load factor reaches 0.7, the capacity is doubled.
It handles the REPL, so the user can enter a string from the command line and it will be tokenized.

## Usage

To compile the program:

```bash
cd src
make
```

This command creates a binary called ./minidb_debug. It compiles with AddressSanitizer and UBSan, so it detects memory errors such as out-of-bounds reads and writes (on the stack, heap and globals)
```bash
make debug
```

To compile with the leaks tool you have to launch
```bash
make leaks
```
make leaks will work only on a macOS system.

Once compiled, run it with the following command
```bash
$ ./minidb
```
At the moment, the program only splits the input string into tokens, one per word.
```bash
> SET nome marco
Token: [SET], [nome], [marco]
```

## Test

The project contains the file test.c, which holds a series of tests to prevent regressions. To run the tests, execute this command

```bash
make test
```
Some tests are in a file called test_repl.sh:
```bash
./test_repl.sh ./minidb_debug
```


## Roadmap
The project currently has many limitations that I aim to solve in future development:
- Currently, on user input the program just splits the words into tokens without handling any command. The goal is to handle the main commands such as
```
> SET nome marco    →  OK
> GET nome          →  marco
> GET boh           →  (nil)
> DEL nome          →  (1)
> DEL nome          →  (0)
> PIPPO             →  error: unknown command
> SET nome          →  error: wrong number of arguments
```
- There is no data persistence: when the program is shut down, restarted or crashes, the data structure is emptied

## Acknowledgements

Written while following [antirez's C course](https://www.youtube.com/playlist?list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY). I used Claude  as a reviewer while learning: it pointed out bugs and design
issues, and wrote the Makefile and the original hash table test suite in
`test.c`. The hash table, tokenizer and REPL code are my own.