#ifndef NASH_H
#define NASH_H

#include <Arduino.h>

#define ON 1
#define OFF 0

/* Options */

/* Prompt title */
#ifndef NASH_PROMPT
#define NASH_PROMPT "nash"
#endif

/* Enable/disable serial remote echo */
#ifndef SERIAL_ECHO
#define SERIAL_ECHO ON
#endif

/* UART baudrate */
#ifndef SERIAL_BAUDRATE
#define SERIAL_BAUDRATE	115200
#endif

/* UART end of line char */
#ifndef SERIAL_EOL
#define SERIAL_EOL 13
#endif

/* Maximum size for input shell line */
#ifndef NASH_LINE_SIZE
#define NASH_LINE_SIZE 64
#endif

/* Maximum number of command line argument including the command itself. */
#ifndef NASH_MAX_ARGS
#define NASH_MAX_ARGS 32
#endif

/* Busy LED pin */
#ifndef NASH_TASK_LED_PIN
#define NASH_TASK_LED_PIN LED_BUILTIN
#endif


/* print */
#define PRINT(...) Serial.print(__VA_ARGS__)
#define WRITE(...) Serial.write(__VA_ARGS__)
#define PRINTLN(...) Serial.println(__VA_ARGS__)
#define ERROR   PRINT
#define ERRORLN PRINTLN
#define PRINT_PROMPT() PRINT(NASH_PROMPT"$ ")

/* Status */
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define ALIVE -1

/* Signals */
#define NOSIGNAL 0
#define SIG_INT	3
#define SIG_EOF	4
#define SIG_ESC	27 
#define SIG_BACKSPACE 127
#define SIG_NEWLINE SERIAL_EOL
typedef unsigned char signal_t;

/* Command */
struct command {
	char buff[NASH_LINE_SIZE + 1];
	size_t argc;
	char **argv;
};

/* Executable */
typedef int(*execloop)(size_t, char **, struct process*);
struct executable {
	const char *name;
    const char *usage;
	execloop worker;
};

/* Process */
struct process {
	struct command *command;
    struct executable *executable;
	execloop worker;
	int status;
	signal_t signal;
	unsigned long starttime;
	
	/* Standard input */
	char input[NASH_LINE_SIZE + 1];
	size_t inlen;

};

void nash_init(struct executable *programs);
void nash_loop();
void nash_help();
void nash_free();
void nash_print_usage(struct executable *exec);
#endif
