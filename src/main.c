#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include "string_util.h"


#define BUF_SIZE 1024 
#define MAX_TOKENS 3

int main(void){
   char buf[BUF_SIZE];

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
     printf("%s\n",buf);
     char *token[MAX_TOKENS];
     size_t count = tokenize_string(buf, token, MAX_TOKENS);
     if(count > MAX_TOKENS){
        printf("Il massimo di token è %d, sono stati inseriti un numero di token superiore. \n",MAX_TOKENS);
        continue;
     }
     printf("Token: ");
     for(size_t i = 0; i<count; i++){
        if(i == count - 1){
            printf("[%s]",token[i]);
        }else{
            printf("[%s], ",token[i]);
        }
     }
     printf("\n");
   }
   printf("\n");

}
