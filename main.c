#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "task.h"
#include "exec.h"

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

        if(strcmp(tokens[0], "exit")==0){
            break;
        }

        if(strcmp(tokens[0], "task")==0){
            if(ntokens<3){ //minimo seria task + nome + programa
                fprintf(stderr, "processflow: uso: task <nome> <programas> [args]\n");
                continue;
            }
            task_cadastrar(tokens[1], &tokens[2], ntokens-2);
            continue;
        }

        if(strcmp(tokens[0], "run")==0){
            if(ntokens!=2){ //run aceita exatamente 1 nome
                fprintf(stderr, "processflow: uso: run <nome>\n");
                continue;
            }
            Task *t = task_buscar(tokens[1]);
            if(t==NULL){ //erro não-fatal: avisa e continua
                fprintf(stderr, "processflow: tarefa '%s' não existe\n", tokens[1]);
                continue;
            }
            exec_rodar(t);
            continue;
        }
        fprintf(stderr, "processflow: comando desconhecido %s\n", tokens[0]);
    }

    return 0;
}