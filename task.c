#include "task.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

Task tarefas[MAX_TAREFAS]; //definição da tabela global

static Task *buscar_slot_livre(void){
    for(int i=0; i<MAX_TAREFAS; i++){
        if(!tarefas[i].ativa){
            return &tarefas[i];
        }
    }
    return NULL;
}

int task_cadastrar(const char *nome, char **tokens, int ntokens){
    if(task_buscar(nome)!=NULL){ //validação 1: nome duplicado
        fprintf(stderr, "processflow: tarefa '%s' ja existe\n", nome);
        return -1;
    }
    
    if(ntokens<1){ //validação 2: sem programa não há o que cadastrar
        fprintf(stderr, "processflow: uso: task <nome> <programa> [args]\n");
        return -1;
    }

    Task *t = buscar_slot_livre();

    if(t==NULL){ //validação 3: tabela cheia
        fprintf(stderr, "processflow: limite de %d tarefas atingidas\n", MAX_TAREFAS);
        return -1;
    }
    snprintf(t->nome, sizeof(t->nome), "%s", nome); //cópia segura: corta e garante o \0

    t->argc=0;
    for(int i=0; i<ntokens&&t->argc<MAX_ARGS; i++){ //copia cada token pra dentro da struct (args[0]=programa)
        snprintf(t->args[t->argc], MAX_ARG_LEN, "%s", tokens[i]);
        t->argc++;
    }

    t->ativa = true;
    return 0;
}

Task *task_buscar(const char *nome){ //busca linear por nome; NULL se não achar
    for(int i=0; i<MAX_TAREFAS; i++){
        if(tarefas[i].ativa && strcmp(tarefas[i].nome, nome) == 0){
            return &tarefas[i];
        }
    }
    return NULL;
}
