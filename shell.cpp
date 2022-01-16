#include "shell.h"


static char buff[SERIAL_LINE_SIZE];
static size_t bufflen = 0;


signal_t shell_readline(char **out, size_t *outlen) {
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
        
		*out = buff;
		*outlen = bufflen;
		buff[bufflen] = 0;
		bufflen = 0;
        return SIG_NEWLINE;
	}
	/* Backspace */
	else if (in == 127) {
		if (bufflen) {
		    WRITE("\b \b");
			bufflen--;
		}
	}
	/* Escape */
	else if (in == SIG_ESC) {
		/* Do Nothing */
		return in;
	}
	/* INTR signal */
	else if (in == SIG_INT) {
		/* Read signal */
		return in;
	}
	/* Line max */
	else if (bufflen < SERIAL_LINE_SIZE) {
#ifdef SERIAL_ECHO
		WRITE(in);
#endif
		buff[bufflen] = in;
		bufflen++;
	}
	return NOSIGNAL;
}


void shell_init() {
	Serial.begin(SERIAL_BAUDRATE);
	Serial.setTimeout(SERIAL_TIMEOUT);
	delay(SERIAL_INIT_DELAY);
	PRINT_PROMPT();
}
