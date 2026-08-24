CC = gcc
CFLAGS = -Wall -Wextra -std=c99
OBJS = main.o task.o exec.o redirect.o job.o

all: processflow

processflow: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o processflow

%.o: %.c task.h exec.h redirect.h job.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f processflow $(OBJS)

test: processflow
	bash run_tests.sh

.PHONY: all clean test
