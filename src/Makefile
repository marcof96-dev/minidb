CC := cc
CFLAGS := -std=c11 -Wall -Wextra -g

# flag extra per la build di debug: sanitizer di memoria + undefined behavior
SAN := -fsanitize=address,undefined -fno-omit-frame-pointer

SRC := main.c hashtable.c string_util.c xmalloc.c
OBJ := $(SRC:.c=.o)

# i test hanno un loro main(), quindi usano la libreria senza main.c
LIB_SRC := hashtable.c string_util.c xmalloc.c
TEST_SRC := test.c $(LIB_SRC)

minidb: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

# regola implicita: ogni .o si costruisce dal .c omonimo
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# build instrumentata: compila i sorgenti in un colpo solo, niente .o condivisi
# con la build normale (non si mescolano oggetti con/senza sanitizer)
debug: $(SRC)
	$(CC) $(CFLAGS) $(SAN) -o minidb_debug $(SRC)

# esegue la build normale sotto `leaks` (ASan su Apple Silicon non rileva i leak)
leaks: minidb
	leaks --atExit -- ./minidb

# compila i test con i sanitizer e li esegue: fallisce se un test fallisce
# o se ASan/UBSan trovano un errore. Lo stdout (i printf di debug della
# libreria) viene scartato; i risultati dei test sono su stderr.
test: $(TEST_SRC)
	$(CC) $(CFLAGS) $(SAN) -o test_minidb $(TEST_SRC)
	./test_minidb > /dev/null

clean:
	rm -f $(OBJ) minidb minidb_debug test_minidb

.PHONY: clean debug leaks test
