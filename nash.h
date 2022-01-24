#ifndef NASH_H
#define NASH_H

#include <Arduino.h>

/* print */
#define PRINT(...) Serial.print(__VA_ARGS__)
#define WRITE(...) Serial.write(__VA_ARGS__)
#define PRINTLN(...) Serial.println(__VA_ARGS__)
#define ERROR   PRINT
#define ERRORLN PRINTLN

/* Status */
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define ALIVE -1

/* Stuff to report available memory */
#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else 
extern char *__brkval;
#endif

/* Serial Options */
#define SERIAL_EOL 13
#define SERIAL_ECHO

/* Signals */
#define NOSIGNAL 0
#define SIG_INT	3
#define SIG_EOF	4
#define SIG_ESC	27 
#define SIG_BACKSPACE 127
#define SIG_NEWLINE SERIAL_EOL

/* Maximum number of command line arguments.  */
#define NASH_MAX_ARGS 16

/* Maximum line size */
#define NASH_LINE_SIZE 64


#define OFF LOW
#define ON HIGH


class Nash {
public:
    
    /* Signal type */
    typedef uint8_t signal_t;

    struct executable;

    /* Process */
    typedef struct {
        struct executable *executable;

        char *buff;
    	uint8_t argc;
    	char **argv;

    	signal_t signal;
    	unsigned long starttime;
    	
    	/* Standard input */
    	char *input;
    } Process;

    /* Executable */
    typedef struct executable {
    	const char *name;
        const uint8_t minArgs;
        const uint8_t maxArgs;
        const char *usage;
        int(*func)(Process*);
    } Executable;
    
    /* Fields */
    char *prompt;

    /* Constructor */
    Nash(const char * prompt_ = "nash-$ ", uint8_t busyLED = LED_BUILTIN);
    
    void init(Executable *programs);
    void loop();
    static void printUsage(Executable *exec, bool error);

protected:
    
    Executable *_programs;
    Process *_currentProcess;
    uint8_t _busyLED;

    /* User input buffer */
    char buff[NASH_LINE_SIZE];
    uint8_t bufflen = 0;

    void _printHelp();
    void _freeProcess();
    void _printPrompt();
    bool _newProcess();
    signal_t _readline();
};


/* Utility to print available memory */
void printFreeMemory();

#endif
