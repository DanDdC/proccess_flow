#ifndef EXEC_H
#define EXEC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "task.h"

int exec_rodar(Task *t);
int exec_sequencial(char **nomes, int n);
int exec_paralelo(char **nomes, int n);
int exec_pipeline(char **nomes, int n);

#endif