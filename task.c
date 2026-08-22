#include "task.h"
#include <stdio.h>
#include <string.h>

Task tarefas[MAX_TAREFAS];

Task *task_buscar(const char *nome){
    for(int i=0; i<MAX_TAREFAS; i++){
        if(tarefas[i].ativa && strcmp(tarefas[i].nome, nome) == 0){
            return &tarefas[i];
        }
    }
    return NULL;
}
