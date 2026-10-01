#pragma once
struct ht_item;
struct hash_table;
struct hash_table *init_ht(void);
void free_ht(struct hash_table *ht);
void insert_item(struct hash_table *hash_table, char *key, char *value);
const char *get_value(struct hash_table *ht, char* key);
void delete_item(struct hash_table *ht, char *key);
int get_counting(struct hash_table *ht);
double load_factor(struct hash_table *ht);
int get_capacity(struct hash_table *ht);
