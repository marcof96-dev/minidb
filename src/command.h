#pragma once
#include<stddef.h>
#include <stdio.h>
struct hash_table;
/*this method executes the command passed by a user input described in tokens variable.
* aof a pointer of a File where to write commands. If its value is NULL there is no need to write into the file.
* ht is the hash_table in which commands are applied.
* tokens represent the command (e.g [Get] [name])
* count is the number of tokens.
* A String that describes the outcome of the command is returned.
* Do not free it. it's valid until the next command that modifies the table.
* Possible returned values:
* SET -> "OK"
* GET -> Value linked to the key or (nil).
* DEL -> (1) if there is the key to remove in the hash table, (0) otherwise
* possible error messages returned:
* - Error: zero token passed! -> if count is 0.
* - Error: unknown command -> if you passed as token[0] a string not contained in the table of command.
* - Error: wrong number of parameters passed -> if the command is reconized but number of expected parameters is wrong
* - Error: command not managed yet! -> if you pass a command not managed 
*/
const char *execute(FILE *aof,struct hash_table *ht, char * tokens[], size_t count);

/*This method tokenize the string passed in buf param and calls the execute method.
* aof a pointer of a File where to write commands. If its value is NULL there is no need to write into the file.
* ht is the hash_table in which tokenized commands are applied.
* buf represent the string to tokenize.
* buf is edited in the method because to tokenize the string is called the function strtok that adds /0 at the end of the String, so it's not possible pass at the function a literal String.
* A String that describes the outcome of the command is returned. Do not free it. it's valid until the next command that modifies the table.
* if buf is empty or contains only spaces NULL is returned.
* Possible error returned:
* - Error: Too many tokens! -> if the number of token is greater than 3.
*/
const char *execute_line(FILE *aof,struct hash_table *ht,char *buf);