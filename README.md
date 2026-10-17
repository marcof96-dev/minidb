# minidb

minidb is a toy key-value store written in C.
It was implemented from scratch for study purposes. I followed [antirez's C course](https://www.youtube.com/playlist?list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY) for many weeks, and this project was born from the desire to put his teachings into practice.
The store is built on a hash table that handles insertion, update, deletion and retrieval of data; collisions are handled through a Separate Chaining algorithm.
It resizes itself automatically: once the load factor reaches 0.7, the capacity is doubled.
It has a REPL: the user types a command on the command line, the input is split into tokens and the command is executed on the hash table.

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

To run the program under the leaks tool and check for memory leaks, launch
```bash
make leaks
```
make leaks will work only on a macOS system.

Once compiled, run it with the following command
```bash
$ ./minidb
```

### Commands

| Command           | Description                          | Reply                          |
|-------------------|--------------------------------------|--------------------------------|
| `SET key value`   | Inserts the key, or updates its value | `OK`                          |
| `GET key`         | Returns the value of the key          | the value, or `(nil)` if the key does not exist |
| `DEL key`         | Deletes the key                       | `(1)` if it was deleted, `(0)` if it did not exist |

Command names are case insensitive (`set`, `SET` and `SeT` are the same command), while keys and values are case sensitive.

Example session:
```
> SET nome marco
OK
> get nome
marco
> GET boh
(nil)
> DEL nome
(1)
> DEL nome
(0)
> PIPPO a
Error: unknown command
> SET nome
Error: wrong number of parameters passed
```

Press Ctrl-D to exit.

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
- There is no data persistence: when the program is shut down, restarted or crashes, the data structure is emptied
- Keys and values cannot contain spaces: `SET nome foo bar` is rejected because the input is split on whitespace
- An input line can be at most 1022 characters long
- Strings are not binary safe: the input cannot contain NUL bytes
- A value that is literally `(nil)` cannot be told apart from a missing key

## Acknowledgements

Written while following [antirez's C course](https://www.youtube.com/playlist?list=PLrEMgOSrS_3cFJpM2gdw8EGFyRBZOyAKY). I used Claude  as a reviewer while learning: it pointed out bugs and design
issues, and wrote the Makefile and the original hash table test suite in
`test.c`. The hash table, tokenizer, REPL and command code are my own.