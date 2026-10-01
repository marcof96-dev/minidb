#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "string_util.h"
#include "xmalloc.h"


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

static struct ht_item *find_in_bucket(struct hash_table *ht, char* key, size_t index){
    struct ht_item *item = ht -> item[index];
    while(item){
        if(my_str_are_equals(item -> key, key)) return item;
        item = item -> next;
    }
    return item;
}



static size_t hash(char *str, size_t num_of_buckets){
    size_t hash = 5381;
    int c;
    while((c = *str++)){
        hash = ((hash<<5)+hash) + c;
    }
    return hash % num_of_buckets;
}

static struct ht_item *init_item(char *key, char *value){
    struct ht_item  *item = xmalloc(sizeof(struct ht_item));
    item -> key = cp_string(key);//copio il valore della stringa per renderlo indipendente dal puntatore passato
    item -> value = cp_string(value);
    item -> next = NULL; //Inizializzo a null
    return item;
}

//Initializing hash_table struct. Setting a fixed size in start phase of the  project
struct hash_table *init_ht(void){
    struct hash_table *ht = xmalloc(sizeof(struct hash_table));
    ht -> capacity = 5; //100 per ora è un placeholder
    ht -> item = xcalloc(ht -> capacity,sizeof(struct ht_item*));
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
    struct hash_table *new_ht = xmalloc(sizeof(struct hash_table));
    new_ht -> capacity = 2*ht->capacity; //100 per ora è un placeholder
    new_ht -> item = xcalloc(new_ht -> capacity,sizeof(struct ht_item*));
    new_ht -> counting = 0;
    for(int i = 0; i<ht -> capacity; i++){
        struct ht_item *item = ht -> item[i];
        if(item != NULL){
            for (; item != NULL; item = item->next) {
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
    size_t index = hash(key, ht -> capacity);
    printf("la chiave %s inserita nell'indice %zu \n",key, index);
        
    struct ht_item *item_checked = find_in_bucket(ht, key,index);
    if(item_checked != NULL){
        printf("Item con chiave %s trovato!\n",key);
        free(item_checked -> value);
        item_checked -> value = cp_string(value);
    }else{
        double lf = load_factor(ht);
        if(lf > 0.7){
            resize_hash_table(ht);
            index = hash(key, ht -> capacity); //Dopo il resize devo ricalcolare l'indice
        }
        struct ht_item *item = init_item(key, value);
        printf("Item con chiave %s e valore %s aggiunto!\n",key,value);
        item -> next = ht->item[index];
        ht->item[index] = item;
        ht -> counting ++;
    }

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




static struct ht_item *get_item(struct hash_table *ht, char* key){
    size_t index = hash(key, ht -> capacity);
    printf("la chiave %s si trova nell'indice %zu \n",key, index);
    return find_in_bucket(ht, key, index);
}

const char *get_value(struct hash_table *ht, char* key){
    struct ht_item *item = get_item(ht, key);
    if(item){
        return item -> value;
    }
    return NULL;
}

static void free_chain(struct ht_item *item){
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




