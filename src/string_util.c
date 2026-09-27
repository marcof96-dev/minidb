#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "string_util.h"

size_t mia_strlen(const char *s){

    size_t len = 0;
     while(*s){
         len ++;
         s++;
      }
    return len;
}

char *cp_string(char *s){
    size_t len = mia_strlen(s);
    char *s_copy = malloc(len +1);
    memcpy(s_copy, s, len + 1 );
    return s_copy;

}

int my_str_are_equals(const char *s1,const char *s2){

   while (*s1 == *s2) {
    if (*s1 == '\0') return 1;   // finite insieme → uguali
    s1++;
    s2++;
}
return 0;

}
