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

#define NOSIGNAL     0
#define SIG_INT      3
#define SIG_EOF      4
#define SIG_ESC     27 
#define SIG_BACKSPACE 127
#define SIG_NEWLINE SERIAL_EOL
typedef unsigned char signal_t;

struct command {
    char buff[SERIAL_LINE_SIZE + 1];
    size_t argc;
    char **argv;
};


signal_t shell_readline(char **out, size_t *outlen);
void shell_init();
void back_to_prompt(struct command *cmd);
#endif
