#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include "string_util.h"
#include "command.h"
#include "aof.h"

#define BUF_SIZE 1024 


int main(int argc, char *argv[]){
   char buf[BUF_SIZE];
   struct hash_table *ht = init_ht();
   FILE *aof_file = aof_open(argc > 1 ? argv[1] : "minidb.aof");
   if(aof_file==NULL){
      perror("Unable to open file");
      return 1;
   }
   int count = aof_replay(ht,aof_file);
   if(count == -1){
      perror("Unable to execute file");
      return 1;
   }
   while(1){
    printf("> ");
     fflush(stdout);
     
     // Stop when the input ends (EOF, e.g. Ctrl-D).
     // fgets + strlen can't handle NUL bytes in the input: to be rewritten with binary-safe strings.
     if(fgets(buf, sizeof(buf),stdin) == NULL){
        break;//Input finito
     }

     size_t str_len = mia_strlen(buf);
     if(str_len == 0){
        printf("Errore dovuto alla a lunghezza nulla della stringa \n");
        continue;
     }

     if(str_len == 1 && buf[0] == '\n'){
        continue;
     }

     if(buf[str_len - 1] != '\n' && !feof(stdin)){
        printf("Valore troppo grande! Non è possibile inserire più di %d caratteri \n", (BUF_SIZE - 2));//Metto il -2 perchè devo contare il /n e il /0
        int c;
        do{
            c = getchar();
        }while(c !='\n' && c!=EOF);
        continue;
     }

     buf[strcspn(buf, "\n")] = 0; // Remove the trailing newline
     
     const char *outcome = execute_line(aof_file,ht,buf);
     if(outcome == NULL){
      continue;
     }
     printf("%s\n", outcome);
   }
   printf("\n");
   free_ht(ht);
   aof_close_file(aof_file);
   return 0;
}
