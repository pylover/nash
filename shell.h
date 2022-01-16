#ifndef SHELL_H
#define SHELL_H

#include "options.h"
#include <Arduino.h>

/* print */
#define PRINT(...) Serial.print(__VA_ARGS__)
#define WRITE(...) Serial.write(__VA_ARGS__)
#define PRINTLN(...) Serial.println(__VA_ARGS__)

#define ERROR   PRINT
#define ERRORLN PRINTLN

#define PRINT_PROMPT() PRINT(PROMPT"$ ")


struct command {
    char buff[SERIAL_LINE_SIZE + 1];

    char *name;
    char *args;
    size_t argslen;
};


struct command * shell_loop();
void shell_init();
#endif
