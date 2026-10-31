#pragma once
#include <stdio.h>
struct hash_table;
/*Open aof file if exists otherwise the method creates the file.
* If it fails returns NULL
*/
FILE *aof_open(const char *path);

/*This method writes in the aof_file (file with extension .aof) every string stored in tokens[] variable separated each string by a blank space
 * Params:
 * - aof_file -> the file where to write.
 * - tokens[] -> array of String. Every bucket contains a single word of the user input.
 * - count -> number of tokens
 * Returned values:
 * - 1 if the outcome of the writing to the file is positive
 * - 0 otherwise
*/
int aof_write_row(FILE *aof_file, char *tokens[], size_t count);

/* This method executes commands in the aof file and fill or remove elements in the struct hash_table.
* Params:
* - aof_file -> the file where read the commands
* - ht -> struct hash_table to manage
* Returned values:
* - number of executed commands
* - 0 if the none commands is executed
* - If the last row of the file is truncated will be removed and it will not counted in the return value
*/
int aof_replay(struct hash_table *ht, FILE *aof_file);
/*method that closes the aof file.
* Params:
* aof -> the file to close
* Returned values:
* an int to manage std fclose error
*/
int aof_close_file(FILE *aof);
