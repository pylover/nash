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
    size_t argc;
    char **argv;
};


struct command * shell_loop();
void shell_init();
void shell_prompt(struct command *cmd);
#endif
