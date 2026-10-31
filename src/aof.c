#include <stdio.h>
#include <string.h>
#include "string_util.h"
#include "command.h"
#include "aof.h"

FILE *aof_open(const char *path){
    FILE *aof_file = fopen(path, "a+");
    return aof_file;
}

int aof_write_row(FILE *aof_file, char *tokens[],size_t count){
    return 1;
}

static int is_last_char_newline(FILE *aof_file){
    return 1;
}

static int delete_last_row(FILE *aof_file){
    return 1;
}

int aof_replay(struct hash_table *ht, FILE *aof_file){
    int count = 0;
    int row = 0;
    rewind(aof_file);
    char buf[1024];
    while(fgets(buf,sizeof(buf),aof_file)){
        row++;
        buf[strcspn(buf, "\n")] = 0;
        const char *outcome = execute_line(NULL, ht, buf);
        if(outcome == NULL){
            continue;
        }
        if(my_str_startwith(outcome, "Error") == 1){
            fprintf(stderr, "Error on row %i \n", row);
            continue;
        }
        count++;
    }
    fseek(aof_file, 0, SEEK_END);
    return count;
}


int aof_close_file(FILE *aof){
    return 1;
}


