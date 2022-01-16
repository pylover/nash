#ifndef NASH_H
#define NASH_H

#include "options.h"
#include "shell.h"


/* STATUS */
#define EXIT_FAILURE   1
#define EXIT_SUCCESS   0
#define ALIVE         -1


typedef unsigned char signal_t;

typedef int(*execloop)(size_t, char **, signal_t);

struct executable {
    const char *name;
    execloop worker;
};

struct process {
    struct command *command;
    execloop worker;
    int status;
    signal_t signal;
};

void nash_init(struct executable *programs);
void nash_loop();
void nash_help();
#endif

