#ifndef TASK_H
#define TASK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TAREFAS 32
#define MAX_NOME 64
#define MAX_ARGS 32
#define MAX_ARG_LEN 256

typedef struct{ //uma tarefa cadastrada
    char nome [MAX_NOME]; //chave de busca
    char args [MAX_ARGS][MAX_ARG_LEN]; //cópia dos tokens (args[0]=programa)
    int argc; //numero de strings válidas em args
    bool ativa; //slot ocupado
} Task;

extern Task tarefas[MAX_TAREFAS]; //a memória real fica no task.c

int task_cadastrar(const char *nome, char **tokens, int ntokens);
Task *task_buscar(const char *nome);

#endif