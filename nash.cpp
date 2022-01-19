#include "nash.h"


Nash::Nash(const char * prompt, uint8_t busyLED) {
	_prompt = prompt;
	_busyLED = busyLED;	
}


void
Nash::init(Executable *programs) {
	_programs = programs;

	/* GPIO */
	pinMode(_busyLED, OUTPUT);
	digitalWrite(_busyLED, OFF);

	/* Initial Prompt */
	_printPrompt();
}


void 
Nash::_printPrompt() {
	_printPrompt(NULL);
}


void 
Nash::_printPrompt(Command *cmd) {
 	if (cmd != NULL) {
 		free(cmd->argv);
 		free(cmd);
 	}
	digitalWrite(_busyLED, OFF);
	PRINT(_prompt);
	PRINT("$ ");
}


void
Nash::loop() {
	/* Process running task if any. */
	if (_current.status == ALIVE) {
		/* Process is running */
		Command *cmd = _current.command;
		_current.status = _current.executable->func(
				cmd->argc, 
				cmd->argv, 
				&_current
			);
		if (_current.status != ALIVE) {
			/* Process has been terminated */
			_printPrompt(_current.command);
		}
	}
	
	/* read a line from input if any. */
	char *line;
	uint8_t linelen = 0;
	signal_t signal = _readline(&line, &linelen);
	
	/* New line */
	if (signal == SIG_NEWLINE) {
		if (_current.status == ALIVE) {
			memcpy(_current.input, line, linelen);
			_current.inlen = linelen;
			_current.input[linelen] = NULL;
		}
		else if (linelen == 0) {
			_printPrompt();
		}
		else {
			Command * cmd = _newCommand(line, linelen);	
			if (cmd == NULL) {
				_printPrompt();
				return NULL;
			}
			else {
				_execute(cmd);
			}
		}
	}

	/* Signal */
	else if ((_current.status == ALIVE) && signal) {
		_current.signal = signal;
	}
}


Nash::signal_t 
Nash::_readline(const char **out, uint8_t *outlen) {
	if (Serial.available() <= 0) {
		return NULL;
	}
		
	char in = Serial.read();
	
	/* Uncomment for debug */
	//PRINTLN();
	//PRINTLN(in, DEC);

	switch (in) {
		/* Enter Key */
		case SERIAL_EOL:
	
#ifdef SERIAL_ECHO
			PRINTLN();
#endif	
			*out = buff;
			*outlen = bufflen;
			bufflen = 0;
			return SIG_NEWLINE;

		case SIG_BACKSPACE:
			if (bufflen) {
#ifdef SERIAL_ECHO
				WRITE("\b \b");
#endif	
				bufflen--;
			}
			break;
		
		/* Signals */
		case SIG_ESC:
		case SIG_INT:
		case SIG_EOF:
			return in;
		
		/* Normal characters */
		default:
			/* Line max */
			if (bufflen < NASH_LINE_SIZE) {
#ifdef SERIAL_ECHO
				WRITE(in);
#endif	
				buff[bufflen] = in;
				bufflen++;
			}
	}
	return NOSIGNAL;
}


Nash::Command* 
Nash::_newCommand(const char *line, uint8_t linelen) {
	Command *cmd = malloc(sizeof(Command));
	if (cmd == NULL) {
		ERRORLN("Out of memory");
		return NULL;
	}
	
	/* Copy buffer */
	memcpy(cmd->buff, line, linelen);
	cmd->buff[linelen] = NULL;
	
	/* Parse arguments */
	char *argv[NASH_MAX_ARGS];
	uint8_t argc = 0;
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

		if (argc == NASH_MAX_ARGS) {
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
	return cmd;
}


void 
Nash::_execute(Command *cmd) {

	/* Check for help command*/
	if (strcmp("help", cmd->argv[0]) == 0) {
		_printHelp();
		_printPrompt(cmd);
		return;
	}
	
	/* Find command */
	Executable *exec = _programs;
	while(exec->name != NULL) {
		if (strcmp(exec->name, cmd->argv[0]) == 0) {
			break;	
		}
		exec++;	
	}

	if (exec->name == NULL) {
		ERROR("Command '");
		ERROR(cmd->argv[0]);
		ERRORLN("' was not found.");
		_printPrompt(cmd);
		return;
	}
	
	/* Check for -h/--help */
	if ((exec->usage != NULL) && (cmd->argc == 2) && 
			((strcmp(cmd->argv[1], "-h") == 0) || 
			 (strcmp(cmd->argv[1], "--help") == 0))) {
		printUsage(exec, false);
		_printPrompt(cmd);
		return;
	}
	/* Executing */
	digitalWrite(_busyLED, ON);
	_current.command = cmd;
	_current.executable = exec;
	_current.status = ALIVE;
	_current.signal = NOSIGNAL;
	_current.starttime = millis();
	_current.inlen = 0;
}


static void 
Nash::printUsage(Executable *exec, bool error) {
	if (error) {
		ERRORLN("Invalid number of arguments.");
	}
	PRINT("Usage: ");
	PRINT(exec->name);
	PRINT(" ");
	PRINTLN(exec->usage);
}

 
void 
Nash::_printHelp() {
	Executable *exec = _programs;
	while(exec->name != NULL) {
		PRINT(exec->name);
		PRINT(" ");
		exec++;	
	}
	PRINTLN();
}
 
 
void 
printFreeMemory() {
	int free;
	char top;
#ifdef __arm__
	free = &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	free = &top - __brkval;
#else
	free = __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif
	PRINTLN(free);
}
