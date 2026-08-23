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

static pid_t lancar(Task *t){ //fork+execvp; devolve o pid do filho no pai
    char *argv[MAX_ARGS+1];
    montar_argv(t, argv);

    pid_t pid = fork();
    if(pid<0){
        perror("processflow: fork");
        return -1;
    }
    if(pid==0){ //filho
        execvp(argv[0], argv); //só volta se falhar
        fprintf(stderr, "processflow: %s: programa não encontrado\n", argv[0]);
        _exit(127);
    }
    return pid; //só o pai chega nesta linha
}

int exec_rodar(Task *t){
    pid_t pid = lancar(t);
    if(pid<0){ //nem nasceu
        return -1;
    }

    int status;
    if(waitpid(pid, &status, 0)<0){ //espera ESTE filho terminar
        perror("processflow: waitpid");
        return -1;
    }

    if(WIFEXITED(status)){
        return WEXITSTATUS(status); //devolve o código do filho
    }
    return -1; //morreu por sinal
}

int exec_sequencial(char **nomes, int n){
    for(int i=0; i<n; i++){
        Task *t = task_buscar(nomes[i]);
        if(t==NULL){ //erro não-fatal: pula e continua a fila
            fprintf(stderr, "processflow: tarefa '%s' não existe\n", nomes[i]);
            continue;
        }
        exec_rodar(t);
    }
    return 0;
}

int exec_paralelo(char **nomes, int n){
    int npids=0; //quantos filhos nasceram

    for(int i=0; i<n; i++){ //fase 1: lança todos
        Task *t = task_buscar(nomes[i]);
        if(t==NULL){ //erro não-fatal
            fprintf(stderr, "processflow: tarefa '%s' não existe\n", nomes[i]);
            continue;
        }
        pid_t pid = lancar(t);
        if(pid>0){
            npids++;
        }
    }

    for(int i=0; i<npids; i++){ //fase 2: colhe TODOS, em qualquer ordem
        int status;
        if(waitpid(-1, &status, 0)<0){ //-1: qualquer filho morto serve
            perror("processflow: waitpid");
            break;
        }
    }
    return 0;
}