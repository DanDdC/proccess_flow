#include "exec.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

static void montar_argv(Task *t, char *argv[MAX_ARGS+1]){ //monta o formato que o execvp espera
    for(int i=0; i<t->argc; i++){
        argv[i] = t->args[i];
    }
    argv[t->argc] = NULL;
}

int exec_rodar(Task *t){
    char *argv[MAX_ARGS+1];
    montar_argv(t, argv);

    pid_t pid = fork(); //duplica o processo: daqui em diante existem dois

    if(pid<0){ //fork falhou: nenhum filho nasceu
        perror("processflow: fork");
        return -1;
    }

    if(pid==0){ //filho
        execvp(argv[0], argv); //substitui o corpo pelo programa; só volta se falhar
        fprintf(stderr, "processflow: %s: programa não encontrado\n", argv[0]);
        _exit(127); //127 = convenção universal de "comando não encontrado"
    }

    int status; //daqui pra frente só o pai passa
    if(waitpid(pid, &status, 0)<0){ //espera ESTE filho terminar
        perror("processflow: waitpid");
        return -1;
    }

    if(WIFEXITED(status)){ //terminou normal (exit/return)?
        return WEXITSTATUS(status); //devolve o código do filho
    }
    return -1; //morreu por sinal (kill etc.)
}