#include "task.h"
#include <stdio.h>
#include <string.h>

Task tarefas[MAX_TAREFAS];

static Task *buscar_slot_livre(void){
    for(int i=0; i<MAX_TAREFAS; i++){
        if(!tarefas[i].ativa){
            return &tarefas[i];
        }
    }
    return NULL;
}

int task_cadastrar(const char *nome, char **tokens, int ntokens){
    if(task_buscar(nome)!=NULL){
        fprintf(stderr, "processflow: tarefa '%s' ja existe\n", nome);
        return -1;
    }
    
    if(ntokens<1){
        fprintf(stderr, "processflow: uso: task <nome> <programa> [args]\n");
        return -1;
    }

    Task *t = buscar_slot_livre();

    if(t==NULL){
        fprintf(stderr, "processflow: limite de %d tarefas atingidas\n", MAX_TAREFAS);
        return -1;
    }
    snprintf(t->nome, sizeof(t->nome), "%s", nome);

    t->argc=0;
    for(int i=0; i<ntokens&&t->argc<MAX_ARGS; i++){
        snprintf(t->args[t->argc], MAX_ARG_LEN, "%s", tokens[i]);
        t->argc++;
    }

    t->ativa = true;
    return 0;
}

Task *task_buscar(const char *nome){
    for(int i=0; i<MAX_TAREFAS; i++){
        if(tarefas[i].ativa && strcmp(tarefas[i].nome, nome) == 0){
            return &tarefas[i];
        }
    }
    return NULL;
}
