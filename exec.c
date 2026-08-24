#include "exec.h"
#include "redirect.h"

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

static void avisar_status(const char *quem, int status){ //aviso pra processo que falhou
    if(WIFEXITED(status) && WEXITSTATUS(status)!=0){
        fprintf(stderr, "processflow: aviso: '%s' terminou com código %d\n", quem, WEXITSTATUS(status));
    }
}

pid_t exec_lancar(Task *t){ //fork+execvp; devolve o pid do filho no pai (pública: job.c também usa)
    char *argv[MAX_ARGS+1];
    montar_argv(t, argv);

    pid_t pid = fork();
    if(pid<0){
        perror("processflow: fork");
        return -1;
    }
    if(pid==0){ //filho
        redirect_aplicar_filho(t); //religa stdin/stdout p/ arquivos, se configurados
        execvp(argv[0], argv); //só volta se falhar
        fprintf(stderr, "processflow: %s: programa não encontrado\n", argv[0]);
        _exit(127);
    }
    return pid; //só o pai chega nesta linha
}

int exec_rodar(Task *t){
    pid_t pid = exec_lancar(t);
    if(pid<0){ //nem nasceu
        return -1;
    }

    int status;
    if(waitpid(pid, &status, 0)<0){ //espera ESTE filho terminar
        perror("processflow: waitpid");
        return -1;
    }

    if(WIFEXITED(status)){
        avisar_status(t->nome, status); //avisa antes de devolver o código
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
        pid_t pid = exec_lancar(t);
        if(pid>0){
            npids++;
        }
    }

    for(int i=0; i<npids; i++){ //fase 2: colhe TODOS, em qualquer ordem
        int status;
        pid_t p = waitpid(-1, &status, 0); //-1: qualquer filho morto serve
        if(p<0){
            perror("processflow: waitpid");
            break;
        }
        char quem[32]; //sem mapa pid→nome aqui: avisa pelo pid mesmo
        snprintf(quem, sizeof(quem), "pid %d", (int)p);
        avisar_status(quem, status);
    }
    return 0;
}

int exec_pipeline(char **nomes, int n){
    Task *ts[MAX_TAREFAS];

    int ok=1;
    for(int i=0; i<n; i++){ //valida TODOS antes, um furo no meio quebraria o fluxo
        ts[i]=task_buscar(nomes[i]);
        if(ts[i]==NULL){
            fprintf(stderr, "processflow: tarefa '%s' não existe\n", nomes[i]);
            ok=0;
        }
    }
    if(!ok){
        return -1;
    }

    int pipes[MAX_TAREFAS-1][2]; //n-1 canais; [j][0]=leitura [j][1]=escrita

    for(int j=0; j<n-1; j++){
        if(pipe(pipes[j])<0){
            perror("processflow: pipe");
            return -1;
        }
    }

    for(int i=0; i<n; i++){
        pid_t pid = fork();
        if(pid<0){ //falhou esse elo: segue sem ele
            perror("processflow: fork");
            continue;
        }
        if(pid==0){ //filho i: religa as mangueiras ANTES do exec
            if(i>0){ //recebe do canal anterior
                dup2(pipes[i-1][0], STDIN_FILENO);
            }
            if(i<n-1){ //entrega pro próximo canal
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for(int j=0; j<n-1; j++){ //filho não usa nenhuma ponta ORIGINAL
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            char *argv[MAX_ARGS+1];
            montar_argv(ts[i], argv);
            execvp(argv[0], argv); //substitui pelo programa de verdade
            fprintf(stderr, "processflow: %s: programa não encontrado\n", argv[0]);
            _exit(127);
        }
    }

    for(int j=0; j<n-1; j++){ //pai também solta TODAS as pontas
        close(pipes[j][0]);
        close(pipes[j][1]);
    }

    int status; //colhe exatamente quantos lançou
    for(int i=0; i<n; i++){
        pid_t p = waitpid(-1, &status, 0);
        if(p>0){ //mesmo aviso de falha do paralelo
            char quem[32];
            snprintf(quem, sizeof(quem), "pid %d", (int)p);
            avisar_status(quem, status);
        }
    }
    return 0;
}