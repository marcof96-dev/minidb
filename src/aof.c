#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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

static int is_last_char_newline(char buf[]){
    size_t len = mia_strlen(buf);
    if(len > 0 && buf[len - 1] == '\n'){
        return 1;
    } else {
        return 0;
    }
}

static int delete_last_row(FILE *aof_file, long pos, int row){
    int fd = fileno(aof_file);
    if(fd == -1){
        return -1;
    }
    int flush_outcome = fflush(aof_file);
    if(flush_outcome == EOF){
        return -1;
    }
    int trunc_outcome = ftruncate(fd,pos);
    if(trunc_outcome == -1){
        return -1;
    }
    fprintf(stderr, "Truncated row at row %d\n",row);
    return 0;
}

int aof_replay(struct hash_table *ht, FILE *aof_file){
    int count = 0;
    int row = 0;
    rewind(aof_file);
    char buf[1024];
    long pos = ftell(aof_file);
    while(fgets(buf,sizeof(buf),aof_file)){
        row++;
        int jump_row = 0;
        int truncate_row = 0;
        if(is_last_char_newline(buf) == 0){
            truncate_row = 1;
            while(fgets(buf,sizeof(buf),aof_file)){
                if(is_last_char_newline(buf) == 1){
                    truncate_row = 0;
                    jump_row = 1;
                    break;
                }
                
            }
            
        }
        if(truncate_row == 1){
            int del_outcome = delete_last_row(aof_file, pos,row);
            if(del_outcome == -1){
                return del_outcome;
            }
            break;    
        }
        if(jump_row == 1){
            pos = ftell(aof_file);
            fprintf(stderr, "Error on row %i. Value too long \n", row);
            continue;
        }
        
        buf[strcspn(buf, "\n")] = 0;
        const char *outcome = execute_line(NULL, ht, buf);
        if(outcome == NULL){
            pos = ftell(aof_file);
            continue;
        }
        
        if(my_str_startwith(outcome, "Error") == 1){
            pos = ftell(aof_file);
            fprintf(stderr, "Error on row %i \n", row);
            continue;
        }
        count++;
        pos = ftell(aof_file);
    }
    fseek(aof_file, 0, SEEK_END);
    return count;
}


int aof_close_file(FILE *aof){
    return 1;
}


