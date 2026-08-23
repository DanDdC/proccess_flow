#ifndef REDIRECT_H
#define REDIRECT_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "task.h"

int redirect_input(const char *nome, const char *arquivo);
int redirect_output(const char *nome, const char *arquivo);
int redirect_append(const char *nome, const char *arquivo);
void redirect_aplicar_filho(const Task *t);

#endif