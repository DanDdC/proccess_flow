#include <stdio.h>
#include <stdlib.h>

#define MAX_LINHA 1024

int main(int argc, char *argv[]){
    char linha[MAX_LINHA];

    if(argc>2){
        fprintf(stderr, "uso: %s [workflowFile]\n", argv[0]);
        return 1;
    }
    if(argc==2){
        fprintf(stderr, "!!!!!!!!!!LEMBRAR DE COMPLETAR ESSE PEDACO DO CODIGO!!!!!!!!!!!!\n");
        return 1;
    }

    while(true){
        printf("processflow> ");
        fflush(stdout);

        if(fgets(linha, sizeof(linha), stdin)==NULL){
            printf("\n");
            break;
        }

        printf("%s", linha);
    }

    return 0;
}