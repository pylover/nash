#include "shell.h"


static char buff[SERIAL_LINE_SIZE];
static size_t bufflen = 0;


struct command * newcommand() {
    struct command *cmd = malloc(sizeof(struct command));
    if (cmd == NULL) {
        ERROR("Out of memory");
        return NULL;
    }
    
    /* Copy & reset global buffer for future commands */
	memcpy(cmd->buff, buff, bufflen);
	cmd->buff[bufflen] = NULL;
    
    /* split by first space */
	cmd->name = cmd->buff;
    cmd->args = memchr(cmd->buff, ' ', bufflen);
	cmd->argslen = 0;
	if (cmd->args != NULL) {
		cmd->args[0] = NULL;
		cmd->args++;
		cmd->argslen = bufflen - (cmd->args - cmd->name);
	}
	
	bufflen = 0;
    return cmd;
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
        
        return newcommand();
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
	PRINT_PROMPT();
}
