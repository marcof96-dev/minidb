#pragma once
#include <stddef.h> 
void *xmalloc(size_t size);
void *xrealloc(void *oldptr, size_t size);
void *xcalloc(size_t nmeb, size_t size);