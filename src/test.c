/* Test della hash table.
 *
 * Ogni test_* crea la sua tabella, esegue uno scenario e verifica i
 * risultati con CHECK. Alla fine viene stampato il riepilogo e il
 * programma esce con 1 se almeno un controllo e' fallito, cosi'
 * `make test` fallisce anche lui.
 *
 * I risultati vanno su stderr: su stdout ci sono ancora i printf di
 * debug di hashtable.c, che il Makefile scarta. */

#include <stdio.h>
#include <stdlib.h>
#include "hashtable.h"

static int checks_run = 0;
static int checks_failed = 0;

/* Se la condizione e' falsa stampa file, riga e il testo della condizione.
 * Non interrompe il test: cosi' vedi tutti i fallimenti in un colpo. */
#define CHECK(cond) do {                                                  \
    checks_run++;                                                         \
    if (!(cond)) {                                                        \
        checks_failed++;                                                  \
        fprintf(stderr, "  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    }                                                                     \
} while (0)

#define N_KEYS 300

/* Genera la chiave i-esima ("k0", "k1", ...) nel buffer passato. */
static char *key_n(char *buf, size_t len, int i) {
    snprintf(buf, len, "k%d", i);
    return buf;
}

static void test_empty_table(void) {
    struct hash_table *ht = init_ht();
    CHECK(get_counting(ht) == 0);
    CHECK(get_item(ht, "nope") == NULL);
    free_ht(ht);
}

static void test_insert_and_get(void) {
    struct hash_table *ht = init_ht();
    insert_item(ht, "a", "1");
    insert_item(ht, "b", "2");
    CHECK(get_counting(ht) == 2);
    CHECK(get_item(ht, "a") != NULL);
    CHECK(get_item(ht, "b") != NULL);
    CHECK(get_item(ht, "c") == NULL);
    free_ht(ht);
}

/* Inserire una chiave esistente deve aggiornarla, non duplicarla. */
static void test_update_existing_key(void) {
    struct hash_table *ht = init_ht();
    insert_item(ht, "a", "1");
    insert_item(ht, "a", "2");
    insert_item(ht, "a", "3");
    CHECK(get_counting(ht) == 1);
    CHECK(get_item(ht, "a") != NULL);

    /* Dopo il delete non deve restare un duplicato nascosto. */
    delete_item(ht, "a");
    CHECK(get_counting(ht) == 0);
    CHECK(get_item(ht, "a") == NULL);
    free_ht(ht);
}

/* La chiave che fa scattare il resize deve finire nel bucket giusto
 * della tabella nuova (bug dell'indice non ricalcolato). */
static void test_key_inserted_during_resize(void) {
    struct hash_table *ht = init_ht();
    char buf[16];
    int resizes = 0;
    for (int i = 0; i < N_KEYS; i++) {
        int cap_before = get_capacity(ht);
        insert_item(ht, key_n(buf, sizeof buf, i), "v");
        if (get_capacity(ht) != cap_before) {
            resizes++;
            CHECK(get_item(ht, buf) != NULL);
        }
    }
    CHECK(resizes > 0);   /* altrimenti il test non ha verificato niente */
    free_ht(ht);
}

/* Dopo molti resize tutte le chiavi devono essere ancora raggiungibili,
 * il contatore giusto e il load factor sotto controllo. */
static void test_many_inserts(void) {
    struct hash_table *ht = init_ht();
    char buf[16];
    for (int i = 0; i < N_KEYS; i++)
        insert_item(ht, key_n(buf, sizeof buf, i), "v");

    CHECK(get_counting(ht) == N_KEYS);
    CHECK(load_factor(ht) <= 1.0);
    int missing = 0;
    for (int i = 0; i < N_KEYS; i++)
        if (get_item(ht, key_n(buf, sizeof buf, i)) == NULL) missing++;
    CHECK(missing == 0);
    free_ht(ht);
}

/* Update di chiavi che stanno anche in mezzo alle catene. */
static void test_many_updates(void) {
    struct hash_table *ht = init_ht();
    char buf[16];
    for (int i = 0; i < N_KEYS; i++)
        insert_item(ht, key_n(buf, sizeof buf, i), "old");
    for (int i = 0; i < N_KEYS; i++)
        insert_item(ht, key_n(buf, sizeof buf, i), "new");
    CHECK(get_counting(ht) == N_KEYS);
    free_ht(ht);
}

static void test_delete_missing_key(void) {
    struct hash_table *ht = init_ht();
    delete_item(ht, "nope");              /* tabella vuota */
    CHECK(get_counting(ht) == 0);

    insert_item(ht, "a", "1");
    delete_item(ht, "nope");              /* tabella non vuota */
    CHECK(get_counting(ht) == 1);
    CHECK(get_item(ht, "a") != NULL);
    free_ht(ht);
}

/* Cancellando dalla chiave piu' vecchia si colpiscono nodi in coda e in
 * mezzo alle catene (con l'inserimento in testa le vecchie finiscono in
 * fondo), non solo le teste. */
static void test_delete_all_oldest_first(void) {
    struct hash_table *ht = init_ht();
    char buf[16];
    for (int i = 0; i < N_KEYS; i++)
        insert_item(ht, key_n(buf, sizeof buf, i), "v");

    for (int i = 0; i < N_KEYS; i++) {
        delete_item(ht, key_n(buf, sizeof buf, i));
        CHECK(get_item(ht, buf) == NULL);
    }
    CHECK(get_counting(ht) == 0);
    free_ht(ht);
}

/* Cancellando meta' delle chiavi, l'altra meta' deve restare intatta. */
static void test_delete_half(void) {
    struct hash_table *ht = init_ht();
    char buf[16];
    for (int i = 0; i < N_KEYS; i++)
        insert_item(ht, key_n(buf, sizeof buf, i), "v");
    for (int i = 0; i < N_KEYS; i += 2)
        delete_item(ht, key_n(buf, sizeof buf, i));

    CHECK(get_counting(ht) == N_KEYS / 2);
    int wrong = 0;
    for (int i = 0; i < N_KEYS; i++) {
        int present = get_item(ht, key_n(buf, sizeof buf, i)) != NULL;
        int should_be_present = (i % 2 == 1);
        if (present != should_be_present) wrong++;
    }
    CHECK(wrong == 0);
    free_ht(ht);
}

/* Esegue un test e stampa OK/FAIL confrontando i fallimenti prima e dopo. */
#define RUN(test) do {                                                    \
    int failed_before = checks_failed;                                    \
    test();                                                               \
    fprintf(stderr, "%s %s\n",                                            \
            checks_failed == failed_before ? "OK  " : "FAIL", #test);     \
} while (0)

int main(void) {
    RUN(test_empty_table);
    RUN(test_insert_and_get);
    RUN(test_update_existing_key);
    RUN(test_key_inserted_during_resize);
    RUN(test_many_inserts);
    RUN(test_many_updates);
    RUN(test_delete_missing_key);
    RUN(test_delete_all_oldest_first);
    RUN(test_delete_half);

    fprintf(stderr, "\n%d controlli, %d falliti\n", checks_run, checks_failed);
    return checks_failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
