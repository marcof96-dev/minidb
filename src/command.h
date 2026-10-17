#pragma once
#include<stddef.h>
struct hash_table;
/*this method executes the command passed by a user input described in tokens variable.
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
const char *execute(struct hash_table *ht, char * tokens[], size_t count);