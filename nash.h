#ifndef NASH_H
#define NASH_H

/* print */
#define PRINT(...) Serial.print(__VA_ARGS__)
#define WRITE(...) Serial.write(__VA_ARGS__)
#define PRINTLN(...) Serial.println(__VA_ARGS__)

#define ERROR   PRINT
#define ERRORLN PRINTLN

/* Options */
#define SERIAL_ECHO
#define SERIAL_BAUDRATE		115200
#define SERIAL_TIMEOUT		100
#define SERIAL_INIT_DELAY	300
#define SERIAL_EOL          13          // \r
#define SERIAL_LINE_SIZE    16

#define TASK_LED_PIN		13
#define PROMPT				"nash"

/* STATUS */
#define EXIT_SUCCESS   0
#define EXIT_FAILURE   1
#define RUNNING       -1


typedef unsigned char signal_t;

typedef int(*task)(char *, byte, signal_t sig);

struct command {
    const char *name;
    task task;
};

void nash_init(struct command *commands);
void nash_loop();
void nash_help();
#endif

