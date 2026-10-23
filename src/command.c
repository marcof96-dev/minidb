#include<stddef.h>
#include "command.h"
#include "hashtable.h"
#include "string_util.h"



#define MAX_TOKENS 3

typedef enum {
    CMD_GET,
    CMD_SET,
    CMD_DEL
} Cmd;
/*
* this struct represents the command inserted by user. 
* name param is the name of the command.
* n_args is the number of tokens the command needs, including the command name (e.g. SET key value = 3).
* cmd is the type of command represented by an enum.  
*/
struct command{
    const char *name;
    size_t n_args;
    Cmd cmd;
};

static const struct command command_table[] = {
    {"get",2,CMD_GET},
    {"set",3,CMD_SET},
    {"del",2,CMD_DEL},
};

#define NUM_OF_COMMANDS (sizeof(command_table) / sizeof(command_table[0]))
static const struct command *get_command(const char *name){
    for(size_t i = 0; i<NUM_OF_COMMANDS; i++){
        const struct command *cmd = &command_table[i];
        if(my_str_are_equals_nocase(cmd -> name,name)==1){
            return cmd;
        }

    }
    return NULL;
}

static int validate_token_size(const struct command *cmd, size_t count){
    if(count == cmd ->n_args){
        return 1; 
    }
    return 0;
}

const char *execute_line(struct hash_table *ht,char buf[]){
    char *tokens[MAX_TOKENS];
    size_t count = tokenize_string(buf, tokens, MAX_TOKENS);
    if(count == 0){
        return NULL;
     }
    if(count > MAX_TOKENS){
        return "Error: Too many tokens!";
    }
    return execute(ht, tokens, count);
}

const char *execute(struct hash_table *ht, char * tokens[], size_t count){
    if(count == 0){
        return "Error: zero token passed!";
    }
    const struct command *command = get_command(tokens[0]);

    if(command == NULL){
        return "Error: unknown command";
    }
    if(validate_token_size(command, count) != 1){
        return "Error: wrong number of parameters passed";
    }
    switch (command -> cmd) {
        case CMD_GET:{
            const char *value = get_value(ht,tokens[1]);
            if(value) return value;
            return "(nil)";
        }
            
        case CMD_SET:{
            insert_item(ht,tokens[1], tokens[2]);
            return "OK";
        
        }
            
        case CMD_DEL:{
             int is_deleted = delete_item(ht, tokens[1]);
             return is_deleted == 1 ? "(1)" : "(0)";
        }
           
    } 

    return "Error: command not managed yet!";
   
}