#include <stdio.h>
#include <stdlib.h>
#include "xmalloc.h"

void *xmalloc(size_t size){
    void *ptr = malloc(size);
    if (ptr == NULL)
    {
        fprintf(stderr, "OOM allocating %zu bytes \n", size);
        exit(1);
    }
    return ptr;
}

void *xrealloc(void *oldptr, size_t size){
    void *ptr = realloc(oldptr,size);
    if(ptr == NULL){
        fprintf(stderr, "OOM allocating %zu bytes \n", size);
        exit(1);
    }

    return ptr;

}

void *xcalloc(size_t nmeb, size_t size){
    void *ptr = calloc(nmeb,size);
    if(ptr == NULL){
        fprintf(stderr, "OOM allocating %zu bytes \n", size * nmeb);
        exit(1);
    }

    return ptr;

}