#include "job.h"
#include "exec.h"

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct{ //um job em background
    int id; //identificador impresso pelo start
    bool ativa; //ainda rodando?
    pid_t pid;
    char nome[MAX_NOME]; //cópia do nome da tarefa (só pra listagem)
} Job;

static Job jobs[MAX_TAREFAS]; //tabela global zerada
static int proximo_id = 1; //ids nunca são reaproveitados

void job_colher_zumbis(void){ //colhe todos os filhos mortos, sem esperar pelos vivos
    int status;
    pid_t p;

    while((p = waitpid(-1, &status, WNOHANG)) > 0){ //-1: qualquer filho; WNOHANG: não dorme
        for(int i=0; i<MAX_TAREFAS; i++){
            if(jobs[i].ativa && jobs[i].pid==p){
                jobs[i].ativa=false;
                break;
            }
        }
    }
}

int job_iniciar(Task *t){
    job_colher_zumbis(); //libera slots de quem morreu recentemente

    int slot=-1;
    for(int i=0; i<MAX_TAREFAS; i++){ //busca de slot livre
        if(!jobs[i].ativa){
            slot=i;
            break;
        }
    }
    if(slot<0){ //validação: tabela cheia
        fprintf(stderr, "processflow: limite de %d jobs atingido\n", MAX_TAREFAS);
        return -1;
    }

    pid_t pid = exec_lancar(t); //mesma mecânica do run, MAS sem wait depois
    if(pid<0){
        return -1;
    }

    jobs[slot].id = proximo_id++;
    jobs[slot].ativa = true;
    jobs[slot].pid = pid;
    snprintf(jobs[slot].nome, sizeof(jobs[slot].nome), "%s", t->nome);

    printf("[%d] %d\n", jobs[slot].id, pid); //[jobId] PID
    return jobs[slot].id;
}

void job_listar(void){
    job_colher_zumbis(); //atualiza antes de mostrar
    for(int i=0; i<MAX_TAREFAS; i++){
        if(jobs[i].ativa){
            printf("[%d] %d %s\n", jobs[i].id, jobs[i].pid, jobs[i].nome);
        }
    }
}

int job_esperar(int id){
    job_colher_zumbis(); //o alvo pode ter morrido há pouco

    int alvo=-1;
    for(int i=0; i<MAX_TAREFAS; i++){ //busca linear por id
        if(jobs[i].ativa && jobs[i].id==id){
            alvo=i;
            break;
        }
    }
    if(alvo<0){ //erro não-fatal
        fprintf(stderr, "processflow: job %d não existe\n", id);
        return -1;
    }

    int status;
    if(waitpid(jobs[alvo].pid, &status, 0)<0){ //aqui o pai DORME por esse filho específico
        perror("processflow: waitpid");
        return -1;
    }
    jobs[alvo].ativa=false;
    return 0;
}
