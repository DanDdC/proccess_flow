#include "redirect.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

static Task *buscar_para_redirecionar(const char *nome){ //validação comum aos 3 comandos
    Task *t = task_buscar(nome);
    if(t==NULL){
        fprintf(stderr, "processflow: tarefa '%s' não existe\n", nome);
    }
    return t;
}

int redirect_input(const char *nome, const char *arquivo){
    Task *t = buscar_para_redirecionar(nome);
    if(t==NULL){
        return -1;
    }
    snprintf(t->entrada, sizeof(t->entrada), "%s", arquivo);
    return 0;
}

int redirect_output(const char *nome, const char *arquivo){
    Task *t = buscar_para_redirecionar(nome);
    if(t==NULL){
        return -1;
    }
    snprintf(t->saida, sizeof(t->saida), "%s", arquivo);
    t->anexar = false; //sobrescreve do zero
    return 0;
}

int redirect_append(const char *nome, const char *arquivo){
    Task *t = buscar_para_redirecionar(nome);
    if(t==NULL){
        return -1;
    }
    snprintf(t->saida, sizeof(t->saida), "%s", arquivo);
    t->anexar = true; //acrescenta no fim
    return 0;
}

void redirect_aplicar_filho(const Task *t){ //roda NO FILHO, depois do fork, antes do exec
    if(t->entrada[0]!='\0'){ //há arquivo de entrada?
        int fd = open(t->entrada, O_RDONLY);
        if(fd<0){ //erro não-fatal pro fluxo geral: só este filho morre
            fprintf(stderr, "processflow: %s: não foi possível abrir para leitura\n", t->entrada);
            _exit(1);
        }
        dup2(fd, STDIN_FILENO); //slot 0 aponta pro arquivo
        close(fd); //o original ficou redundante após a cópia
    }
    if(t->saida[0]!='\0'){ //há arquivo de saída?
        int flags = O_WRONLY | O_CREAT | (t->anexar ? O_APPEND : O_TRUNC);
        int fd = open(t->saida, flags, 0644);
        if(fd<0){
            fprintf(stderr, "processflow: %s: não foi possível abrir para escrita\n", t->saida);
            _exit(1);
        }
        dup2(fd, STDOUT_FILENO); //slot 1 aponta pro arquivo
        close(fd);
    }
}