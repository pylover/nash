
#include <Arduino.h>
#include "nash.h"


#define PRINT_PROMPT() PRINT(PROMPT"$ ")

static struct command *commands;
static char line[SERIAL_LINE_SIZE];
static byte linelen = 0;


void execute() {
	char *cmdname = line;
	char *args = strchr(line, ' ');
	byte argslen = 0;
	if (args != NULL) {
		args[0] = NULL;
		args++;
		argslen = linelen - (args - cmdname);
	}
	
	struct command cmd;
	byte counter = 0;
	do {
		cmd = commands[counter++];	
		if (strcmp(cmdname, cmd.name) == 0) {
			break;	
		}
	} while(cmd.name != NULL);

	if (cmd.name == NULL) {
		ERROR("Command '");
		ERROR(cmdname);
		ERRORLN("' was not found.");
	}
}


void nash_help() {
	PRINT("Available Commands: ");
	int counter = 0;
	while (true) {
		struct command cmd = commands[counter++];
		if (cmd.name == NULL) {
			break;
		}
		PRINT(cmd.name);
		PRINT(" ");
	}
	PRINTLN();
}

void nash_loop() {
	if (Serial.available() <= 0) {
		return;
	}
		
	char in = Serial.read();

	/* Enter Key */
	if (in == SERIAL_EOL) {
#ifdef SERIAL_ECHO
		PRINTLN();
#endif
		execute();
		linelen = 0;
		PRINT_PROMPT();
	}
	/* Backspace */
	else if (in == 127) {
		if (linelen) {
		    WRITE("\b \b");
			linelen--;
		}
	}
	/* Line max */
	else if (linelen < SERIAL_LINE_SIZE) {
#ifdef SERIAL_ECHO
		WRITE(in);
#endif
		line[linelen] = in;
		linelen++;
	}
		//else if (in == 3) {
		//	VPRINTLN("^C");
		//	kill = true;
		//}
}


void nash_init(struct command *cmds) {
	commands = cmds;
	Serial.begin(SERIAL_BAUDRATE);
	Serial.setTimeout(SERIAL_TIMEOUT);
	delay(SERIAL_INIT_DELAY);
	pinMode(TASK_LED_PIN, OUTPUT);
	
	PRINT_PROMPT();
}
