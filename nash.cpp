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
Nash::_freeProcess() {
 	if (_currentProcess == NULL) {
		return;
 	}
	if (_currentProcess->buff != NULL) {
		free(_currentProcess->buff);
	}
	if (_currentProcess->argv != NULL) {
		free(_currentProcess->argv);
	}
	free(_currentProcess);
	_currentProcess = NULL;
}


void 
Nash::_printPrompt() {
	_freeProcess();
	digitalWrite(_busyLED, OFF);
	PRINT(_prompt);
	PRINT("$ ");
}


void
Nash::loop() {
	/* Process running task if any. */
	if (_currentProcess != NULL) {
		/* Process is running */
		int8_t status = _currentProcess->executable->func(_currentProcess);
		if (_currentProcess->input != NULL) {
			free(_currentProcess->input);
			_currentProcess->input = NULL;
		}
		if (status != ALIVE) {
			/* Process has been terminated */
			_printPrompt();
		}
	}
	
	/* read a line from input if any. */
	signal_t signal = _readline();
	if (signal == SIG_NEWLINE) {
		/* New line */
		if (bufflen) {
			if (_currentProcess == NULL) {
				/* create new proccess */
				if (!_newProcess()) {
					_printPrompt();
				}
			}
			else {
				_currentProcess->input = malloc(bufflen + 1);
				memcpy(_currentProcess->input, buff, bufflen);
				_currentProcess->input[bufflen] = NULL;
			}
			/* we must use the line here and clean the buffer asap */
			bufflen = 0;
		}
		else {
			_printPrompt();
		}
	}
	/* Signal */
	else if ((_currentProcess != NULL) && signal) {
		_currentProcess->signal = signal;
	}
}


Nash::signal_t 
Nash::_readline() {
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

// TODO: Move it to another module
char *trim(char *str) {
    char *end;
    while(isspace((unsigned char)*str)) {
		str++;
	}

    if(*str == 0) {
        return str;
	}

    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) {
		end--;
	}
    end[1] = NULL;
    return str;
}

bool
Nash::_newProcess() {
	/* Copy buffer into the temporay null terminated line */
	char tmp[bufflen + 1];
	memcpy(tmp, buff, bufflen);
	tmp[bufflen] = NULL;
	
	/* Trim */
	char * cmd = trim(tmp);
	
	/* Split by first space */
	char * firstarg = strchr(cmd, ' ');
	if (firstarg != NULL) {
		firstarg[0] = NULL;
		firstarg++;
	}
	
	/* Find executable */
	/* Check for help command*/
	if (strcmp("help", cmd) == 0) {
		_printHelp();
		return false;
	}
	
	/* Find command */
	Executable *exec = _programs;
	while(exec->name != NULL) {
		if (strcmp(exec->name, cmd) == 0) {
			/* Executable found */
			break;	
		}
		exec++;	
	}
	
	if (exec->name == NULL) {
		ERROR("Command '");
		ERROR(cmd);
		ERRORLN("' was not found.");
		return false;
	}
		
	/* Allocate the process */
	Process *proc = _currentProcess = malloc(sizeof(Process));
	proc->executable = exec;
	proc->signal = NOSIGNAL;
	proc->input = NULL;

	if (firstarg != NULL) {
		uint8_t argslen = strlen(firstarg);
		proc->buff = malloc(argslen + 1);
		if (proc->buff == NULL) {
			ERRORLN("Out of memory");
			return false;
		}
		memcpy(proc->buff, firstarg, argslen);
		proc->buff[argslen] = NULL;
		firstarg = proc->buff;
	}
	else {
		proc->buff = NULL;
	}

	/* Parse arguments, +1 for the command itself */
	char *argv[NASH_MAX_ARGS + 1];
	uint8_t argc = 0;
	argv[argc++] = exec->name;
	if (firstarg != NULL) {
		argv[argc++] = firstarg;
		char *cursor = firstarg;
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

			if (argc == (NASH_MAX_ARGS + 1)) {
				ERRORLN("Too many arguments");
				return false;
			}
			argv[argc++] = cursor;
		}
	}
	
	/* Allocate proc->argv */
	proc->argv = malloc(sizeof(char*) * argc);
	if (proc->argv == NULL) {
		ERRORLN("Out of memory");
		return false;
	}
	memcpy(proc->argv, argv, sizeof(char*) * argc);
	
	/* Check for min max arguments */
	uint8_t _argc = argc - 1;
	if ((_argc < exec->minArgs) || (_argc > exec->maxArgs)) {
		printUsage(exec, true);
		return false;
	}

	/* Check for -h/--help */
	if ((exec->usage != NULL) && (argc == 2) && 
			((strcmp(proc->argv[1], "-h") == 0) || 
			 (strcmp(proc->argv[1], "--help") == 0))) {
		printUsage(exec, false);
		return false;
	}

	/* Update process */
	proc->argc = argc;
	digitalWrite(_busyLED, ON);
	proc->starttime = millis();
	return true;
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
