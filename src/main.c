#include <stdio.h>
#include "hashtable.h"

int main(void){
    // struct ht_item *item = init_item("key", "value");
    struct hash_table *ht = init_ht();
    insert_item(ht,"key", "value");
    insert_item(ht,"pippo", "pluto");
    insert_item(ht,"pippo", "paperino");
    insert_item(ht,"ciao", "mondo");
    insert_item(ht,"kfsdv", "tbmdfm");
    insert_item(ht,"yuiop", "cvbnm");
    printf("Counting: %d \n",get_counting(ht)); //Mi aspetto 6
    struct ht_item *item = get_item(ht, "ciao");
    printf("item trovato all'indirizzo %p \n",item);
    delete_item(ht,"ciao");
    printf("Counting: %d \n",get_counting(ht)); //Mi aspetto 5
    printf("Counting: %d, Capacity: %d, Load factor: %f \n",get_counting(ht), get_capacity(ht), load_factor(ht));
    free_ht(ht);
    printf("Compilazione eseguita123!\n");
}
