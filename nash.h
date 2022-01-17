#ifndef NASH_H
#define NASH_H

#include "options.h"
#include <Arduino.h>

/* print */
#define PRINT(...) Serial.print(__VA_ARGS__)
#define WRITE(...) Serial.write(__VA_ARGS__)
#define PRINTLN(...) Serial.println(__VA_ARGS__)
#define ERROR   PRINT
#define ERRORLN PRINTLN
#define PRINT_PROMPT() PRINT(PROMPT"$ ")

/* STATUS */
#define EXIT_FAILURE   1
#define EXIT_SUCCESS   0
#define ALIVE         -1

/* Signals */
#define NOSIGNAL     0
#define SIG_INT      3
#define SIG_EOF      4
#define SIG_ESC     27 
#define SIG_BACKSPACE 127
#define SIG_NEWLINE SERIAL_EOL
typedef unsigned char signal_t;

/* Command */
struct command {
    char buff[SERIAL_LINE_SIZE + 1];
    size_t argc;
    char **argv;
};

/* Executable */
typedef int(*execloop)(size_t, char **, struct process*);

struct executable {
    const char *name;
    execloop worker;
};

/* Process */
struct process {
    struct command *command;
    execloop worker;
    int status;
    signal_t signal;
    unsigned long starttime;
    
    /* Standard input */
    char input[SERIAL_LINE_SIZE + 1];
    size_t inlen;

};

void nash_init(struct executable *programs);
void nash_loop();
void nash_help();
#endif

