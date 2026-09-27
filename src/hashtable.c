#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "string_util.h"


struct ht_item{
    char *key;
    char *value;
    struct ht_item *next; //puntatore a prossimo elemento della linked list
};


struct hash_table{
    int capacity;
    int counting;
    struct ht_item **item; //Il doppio asterisco è per identificare un array di puntatori
    
};

int get_counting(struct hash_table *ht){
    return ht -> counting;
}

int get_capacity(struct hash_table *ht){
    return ht -> capacity;
}

static size_t hash(char *str, size_t num_of_buckets){
    size_t hash = 5381;
    int c;
    while((c = *str++)){
        hash = ((hash<<5)+hash) + c;
    }
    return hash % num_of_buckets;
}

struct ht_item *init_item(char *key, char *value){
    struct ht_item  *item = malloc(sizeof(struct ht_item));
    if(item == NULL){
        return NULL;
    }
    item -> key = cp_string(key);//copio il valore della stringa per renderlo indipendente dal puntatore passato
    item -> value = cp_string(value);
    item -> next = NULL; //Inizializzo a null
    return item;
}

//Initializing hash_table struct. Setting a fixed size in start phase of the  project
struct hash_table *init_ht(void){
    struct hash_table *ht = malloc(sizeof(struct hash_table));
    if(ht == NULL){
        return NULL;
    }
    ht -> capacity = 5; //100 per ora è un placeholder
    ht -> item = calloc(ht -> capacity,sizeof(struct ht_item*));
    ht -> counting = 0;
    return ht;
}

double load_factor(struct hash_table *ht){
    double lf;
    lf = (double) ht->counting / ht -> capacity;
    return lf;

}


static void resize_hash_table(struct hash_table *ht){
    printf("Facendo il resize...");
    struct hash_table *new_ht = malloc(sizeof(struct hash_table));
    new_ht -> capacity = 2*ht->capacity; //100 per ora è un placeholder
    new_ht -> item = calloc(new_ht -> capacity,sizeof(struct ht_item*));
    new_ht -> counting = 0;
    for(int i = 0; i<ht -> capacity; i++){
        struct ht_item *item = ht -> item[i];
        if(item != NULL){
            for (struct ht_item *item = ht->item[i]; item != NULL; item = item->next) {
                insert_item(new_ht, item->key, item->value);
            }
        }
    }

    const int tmp_size = ht->capacity;
    ht->capacity = new_ht->capacity;
    new_ht->capacity = tmp_size;

    const int tmp_counting = ht->counting;
    ht->counting = new_ht->counting;
    new_ht->counting = tmp_counting;

    struct ht_item **tmp_items = ht->item;
    ht->item = new_ht->item;
    new_ht->item = tmp_items;
    free_ht(new_ht);

}

void insert_item(struct hash_table *ht, char *key, char *value){
    double lf = load_factor(ht);
    if(lf > 0.7){
        //TODO FARE QUI RESIZE DELLA HASHTABLE
        resize_hash_table(ht);
    }
    struct ht_item *item = init_item(key, value);
    size_t index = hash(item -> key, ht -> capacity);
    printf("la chiave %s inserita nell'indice %zu \n",key, index);
    struct ht_item *curr_item = ht -> item[index];
    if(curr_item == NULL){
        ht->item[index] = item;
    }else{
        item -> next = ht->item[index];
         ht->item[index] = item;
    }
    ht -> counting ++;

}

void delete_item(struct hash_table *ht, char *key){
    size_t index = hash(key, ht -> capacity);
    struct ht_item *item = ht -> item[index];
    struct ht_item *tmp = item;
    if(tmp == NULL){
        return;
    }
    printf("key =  %s\n", tmp -> key);
    if (my_str_are_equals(tmp->key, key)) {
        ht->item[index] = tmp->next;   // stacca dal bucket PRIMA
        free(tmp->key);
        free(tmp->value);
        free(tmp);
        ht -> counting --;
        return;
    }

    while(tmp -> next){
         printf("key =  %s", tmp -> next -> key);
        if(my_str_are_equals(tmp -> next -> key, key)){
            printf("trovata stringa uguale! %s, %s", tmp -> next -> key, key);
            struct ht_item *target = tmp->next;
            tmp->next = target->next;      // stacca
            free(target->key);
            free(target->value);
            free(target);
            ht -> counting --;
            break;
        }

        tmp = tmp -> next;
    }

    
}


struct ht_item *get_item(struct hash_table *ht, char* key){
    size_t index = hash(key, ht -> capacity);
    printf("la chiave %s si trova nell'indice %zu \n",key, index);
    struct ht_item *item = ht -> item[index];
    while(item){
        if(my_str_are_equals(item -> key, key)) return item;
        item = item -> next;
    }
    return item;

}


void free_chain(struct ht_item *item){
    struct ht_item *tmp = item;
    while(tmp -> next != NULL){
        struct ht_item *tmp_next = tmp -> next;
        free(tmp->key);
        free(tmp -> value);
        free(tmp);
        tmp = tmp_next;
    }
    free(tmp->key);
    free(tmp -> value);
    free(tmp);
}

void free_ht(struct hash_table *ht){
    for(int i = 0; i < ht -> capacity; i++){
        struct ht_item *item = ht -> item[i];
        if(item != NULL){
            free_chain(item);
        }
    }
    free(ht -> item);
    free(ht);
}




