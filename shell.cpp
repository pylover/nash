#include "shell.h"


static char buff[SERIAL_LINE_SIZE];
static size_t bufflen = 0;


struct command * newcommand() {
    struct command *cmd = malloc(sizeof(struct command));
    if (cmd == NULL) {
        ERRORLN("Out of memory");
        return NULL;
    }
    
    /* Copy & reset global buffer for future commands */
	memcpy(cmd->buff, buff, bufflen);
	cmd->buff[bufflen] = NULL;
    
    /* Parse arguments */
	char *argv[SHELL_MAX_ARGS];
	size_t argc = 0;
	argv[argc++] = cmd->buff;

	char *cursor = cmd->buff;
	while (true) {
		cursor = strchr(cursor, ' ');
		if (cursor == NULL) {
			break;
		}
		cursor[0] = NULL;
		cursor++;
		if (strlen(cursor) == 0) {
			break;
		}

		if (argc == SHELL_MAX_ARGS) {
			ERRORLN("Too many arguments");
			free(cmd);
			return NULL;
		}
		argv[argc++] = cursor;
	}
	
	cmd->argc = argc;
	cmd->argv = malloc(sizeof(char*) * argc);
	if (cmd->argv == NULL) {
        ERRORLN("Out of memory");
		free(cmd);
        return NULL;
	}
	
	memcpy(cmd->argv, argv, sizeof(char*) * argc);
	bufflen = 0;
    return cmd;
}

void shell_prompt(struct command *cmd) {
	if (cmd != NULL) {
		free(cmd->argv);
		free(cmd);
	}
	PRINT_PROMPT();
}

struct command * shell_loop() {
	if (Serial.available() <= 0) {
		return NULL;
	}
		
	char in = Serial.read();
	
	//PRINTLN();
	//PRINTLN(in, DEC);
	/* Enter Key */
	if (in == SERIAL_EOL) {
#ifdef SERIAL_ECHO
		PRINTLN();
#endif
        
        if (bufflen == 0) {
			PRINT_PROMPT();
            return NULL;
        }
        
        struct command * cmd = newcommand();
		if (cmd == NULL) {
			PRINT_PROMPT();
            return NULL;
		}
		return cmd;
	}
	/* Backspace */
	else if (in == 127) {
		if (bufflen) {
		    WRITE("\b \b");
			bufflen--;
		}
	}
	/* Escape */
	else if (in == 27) {
		/* Do Nothing */
	}
	/* Line max */
	else if (bufflen < SERIAL_LINE_SIZE) {
#ifdef SERIAL_ECHO
		WRITE(in);
#endif
		buff[bufflen] = in;
		bufflen++;
	}
		//else if (in == 3) {
		//	VPRINTLN("^C");
		//	kill = true;
		//}
	return NULL;
}


void shell_init() {
	Serial.begin(SERIAL_BAUDRATE);
	Serial.setTimeout(SERIAL_TIMEOUT);
	delay(SERIAL_INIT_DELAY);
	PRINTLN();
	PRINT_PROMPT();
}
