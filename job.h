#ifndef JOB_H
#define JOB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "task.h"

void job_colher_zumbis(void); //coleta filhos terminados sem bloquear
int job_iniciar(Task *t); //start: lança em background e registra
void job_listar(void); //jobs
int job_esperar(int id); //wait

#endif
