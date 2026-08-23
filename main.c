#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_LINHA 1024
#define MAX_TOKENS 32

int main(int argc, char *argv[]){
    char linha[MAX_LINHA];
    char *tokens[MAX_TOKENS];
    int ntokens;

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

        ntokens=0;
        char *tok=strtok(linha, " \t\n"); //primeira chamada
        while(tok!=NULL && ntokens<MAX_TOKENS-1){
            tokens[ntokens]=tok;
            ntokens++;
            tok=strtok(NULL, " \t\n");
        }
        tokens[ntokens]=NULL;

        if(ntokens==0){
            continue;
        }
        fprintf(stderr, "texto temporario %d\n", ntokens);
    }

    return 0;
}