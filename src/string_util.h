#pragma once
#include <stddef.h> 
size_t mia_strlen(const char *s);
char *cp_string(char *s);
size_t tokenize_string(char buf[], char *tokens[],size_t max_tokens);
int my_str_are_equals(const char *s1,const char *s2);
int my_str_are_equals_nocase(const char *s1,const char *s2);
int my_str_startwith(const char *s1, const char *s2);

