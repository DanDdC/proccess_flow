#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h> //pid_t

#include "task.h"

int exec_rodar(Task *t);
pid_t exec_lancar(Task *t); //fork+execvp sem wait: usada pelo run e pelos jobs
int exec_sequencial(char **nomes, int n);
int exec_paralelo(char **nomes, int n);
int exec_pipeline(char **nomes, int n);

#endif