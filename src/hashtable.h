#pragma once
struct ht_item;
struct hash_table;
struct ht_item *init_item(char *key, char *value);
struct hash_table *init_ht(void);
void free_ht(struct hash_table *ht);
void free_chain(struct ht_item *item);
void insert_item(struct hash_table *hash_table, char *key, char *value);
void delete_item(struct hash_table *ht, char *key);
struct ht_item *get_item(struct hash_table *ht, char* key);
int get_counting(struct hash_table *ht);
double load_factor(struct hash_table *ht);
int get_capacity(struct hash_table *ht);
