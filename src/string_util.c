#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "string_util.h"
#include "xmalloc.h"

static const char *DELIMS = " \t\r\n";


size_t mia_strlen(const char *s){

    size_t len = 0;
     while(*s){
         len ++;
         s++;
      }
    return len;
}

size_t tokenize_string(char buf[], char *tokens[], size_t max_tokens){
        size_t count = 0;
        char *tok = strtok(buf, DELIMS);
        
        while(tok != NULL && count < max_tokens){
            tokens[count] = tok;
            tok = strtok(NULL, DELIMS);
            count ++;
        }
        while(tok){
            tok = strtok(NULL, DELIMS);
            count ++;
        }
        
        return count;        
}


char *cp_string(char *s){
    size_t len = mia_strlen(s);
    char *s_copy = xmalloc(len +1);
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
