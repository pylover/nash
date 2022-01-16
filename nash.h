#ifndef NASH_H
#define NASH_H

#include "options.h"
#include "shell.h"


/* STATUS */
#define EXIT_SUCCESS   0
#define EXIT_FAILURE   1
#define RUNNING       -1
#define INITIALIZING  -2


typedef unsigned char signal_t;

typedef int(*execloop)(char *, size_t, signal_t);

struct executable {
    const char *name;
    execloop worker;
};

struct process {
    struct command *command;
    execloop worker;
    int status;
};

void nash_init(struct executable *programs);
void nash_loop();
void nash_help();
#endif

