#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_LINHA 1024

int main(int argc, char *argv[]){
    char linha[MAX_LINHA];

    if(argc>2){ //erro: numero incorreto de argumentos
        fprintf(stderr, "uso: %s [workflowFile]\n", argv[0]);
        return 1;
    }
    if(argc==2){ //modo workflow
        fprintf(stderr, "!!!!!!!!!!LEMBRAR DE COMPLETAR ESSE PEDACO DO CODIGO!!!!!!!!!!!!\n");
        return 1;
    }

    while(true){
        printf("processflow> ");
        fflush(stdout); //prompt sem \n: força exibir antes de ler

        if(fgets(linha, sizeof(linha), stdin)==NULL){ //detecção de EOF
            printf("\n");
            break;
        }

        printf("%s", linha); //eco temporário (so pra ver se ta funcionando)
    }

    return 0;
}