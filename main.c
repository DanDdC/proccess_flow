#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "task.h"
#include "exec.h"
#include "redirect.h"

#define MAX_LINHA 1024
#define MAX_TOKENS 32

int main(int argc, char *argv[]){
    char linha[MAX_LINHA];
    char *tokens[MAX_TOKENS];
    int ntokens;
    FILE *entrada = stdin; //fonte dos comandos: teclado por padrão
    bool interativo = true;

    if(argc>2){ //erro: numero incorreto de argumentos
        fprintf(stderr, "uso: %s [workflowFile]\n", argv[0]);
        return 1;
    }
    if(argc==2){ //modo workflow
        entrada = fopen(argv[1], "r");
        if(entrada==NULL){ //erro FATAL: encerra com código != 0
            fprintf(stderr, "processflow: %s: não foi possível abrir o workflow\n", argv[1]);
            return 1;
        }
        interativo = false;
    }
    
    while(true){
        if(interativo){ //prompt só no modo interativo
            printf("processflow> ");
            fflush(stdout);
        }

        if(fgets(linha, sizeof(linha), entrada)==NULL){ //EOF do teclado OU do arquivo
            if(interativo){
                printf("\n");
            }
            break;
        }

        if(!interativo){ //workflow: ecoa a linha ANTES de processar
            printf("%s", linha);
            fflush(stdout); //garante que o eco sai ANTES da saída dos filhos
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

        if(strcmp(tokens[0],"input")==0 || strcmp(tokens[0],"output")==0 || strcmp(tokens[0],"append")==0){
            if(ntokens!=3){ //comando + tarefa + arquivo, exatos
                fprintf(stderr, "processflow: uso: %s <tarefa> <arquivo>\n", tokens[0]);
                continue;
            }
            if(strcmp(tokens[0],"input")==0){
                redirect_input(tokens[1], tokens[2]);
            }
            else if(strcmp(tokens[0],"output")==0){
                redirect_output(tokens[1], tokens[2]);
            }
            else{
                redirect_append(tokens[1], tokens[2]); //sobra o append
            }
            continue;
        }

        if(strcmp(tokens[0], "workdir")==0){
            if(ntokens!=2){ //exatamente 1 diretório
                fprintf(stderr, "processflow: uso: workdir <diretório>\n");
                continue;
            }
            if(chdir(tokens[1])<0){ //erro não-fatal: avisa e segue
                fprintf(stderr, "processflow: %s: diretório não existe\n", tokens[1]);
            }
            continue;
        }

        if(ntokens>=2 && strcmp(tokens[0], "run")==0 && strcmp(tokens[1], "pipe")==0){ //dentro do run!
            if(ntokens<4){ //pipe exige no mínimo 2 tarefas
                fprintf(stderr, "processflow: uso: run pipe <nome> <nome>...\n");
                continue;
            }
            exec_pipeline(&tokens[2], ntokens-2); //nomes começam depois de "run pipe"
            continue;
        }

        if(strcmp(tokens[0], "run")==0){
            if(ntokens>=2 && strcmp(tokens[1], "sequential")==0){
                if(ntokens<3){ //palavra-chave + pelo menos 1 tarefa
                    fprintf(stderr, "processflow: uso: run sequential <nome>...\n");
                    continue;
                }
                exec_sequencial(&tokens[2], ntokens-2);
                continue;
            }
            if(ntokens>=2 && strcmp(tokens[1], "parallel")==0){
                if(ntokens<3){
                    fprintf(stderr, "processflow: uso: run parallel <nome>...\n");
                    continue;
                }
                exec_paralelo(&tokens[2], ntokens-2);
                continue;
            }
            if(ntokens!=2){ //run simples aceita exatamente 1 nome
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
    if(entrada!=stdin){
        fclose(entrada);
    }
    return 0;
}