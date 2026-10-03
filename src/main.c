#include <stdio.h>
#include <string.h>
#include "hashtable.h"
#include "string_util.h"


#define BUF_SIZE 16 

int main(void){
   char buf[BUF_SIZE];

   while(1){
    printf("> ");
     fflush(stdout);
     
     //Controllo se è vuoto o è finita l'input
     //fgets + strlen non gestiscono byte NUL nell'input: da rivedere con le stringhe binary-safe
     if(fgets(buf, sizeof(buf),stdin) == NULL){
        break;//Input finito
     }

     size_t str_len = mia_strlen(buf);
     if(str_len == 0){
        printf("Errore dovuto alla a lunghezza nulla della stringa \n");
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

     buf[strcspn(buf, "\n")] = 0; //Elimino il carattere di new line
     
     printf("%s\n",buf);
   }
   printf("\n");

}
