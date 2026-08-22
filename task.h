#ifndef TASK_H
#define TASK_H

#include <stdbool.h>

#define MAX_TAREFAS 32
#define MAX_NOME 64
#define MAX_ARGS 32
#define MAX_ARG_LEN 256

typedef struct{
    char nome [MAX_NOME];
    char args [MAX_ARGS][MAX_ARG_LEN];
    int argc;
    bool ativa;
} Task;

extern Task tarefas[MAX_TAREFAS];

int task_cadastrar(const char *nome, char **tokens, int ntokens);
Task *task_buscar(const char *nome);

#endif