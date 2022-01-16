#include "shell.h"


static char buff[SERIAL_LINE_SIZE];
static size_t bufflen = 0;


signal_t shell_readline(char **out, size_t *outlen) {
	if (Serial.available() <= 0) {
		return NULL;
	}
		
	char in = Serial.read();
	
	/* Uncomment for debug */
	//PRINTLN();
	//PRINTLN(in, DEC);

	/* Enter Key */
	switch (in) {
		case SERIAL_EOL:
#ifdef SERIAL_ECHO
			PRINTLN();
#endif
        
			*out = buff;
			*outlen = bufflen;
			buff[bufflen] = 0;
			bufflen = 0;
        	return SIG_NEWLINE;

		case SIG_BACKSPACE:
			if (bufflen) {
			    WRITE("\b \b");
				bufflen--;
			}
			break;
			
		case SIG_ESC:
		case SIG_INT:
		case SIG_EOF:
			return in;
		default:
			/* Line max */
			if (bufflen < SERIAL_LINE_SIZE) {
#ifdef SERIAL_ECHO
				WRITE(in);
#endif
				buff[bufflen] = in;
				bufflen++;
			}
	}
	return NOSIGNAL;
}


void shell_init() {
	Serial.begin(SERIAL_BAUDRATE);
	Serial.setTimeout(SERIAL_TIMEOUT);
	delay(SERIAL_INIT_DELAY);
	PRINT_PROMPT();
}
