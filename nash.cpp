#include "nash.h"

/* Executables, provided by user via init() */
static struct executable *programs;

/* Current running process */
static struct process current;

/* User input buffer */
static char buff[NASH_LINE_SIZE];
static size_t bufflen = 0;

static void 
back_to_prompt(struct command *cmd) {
	if (cmd != NULL) {
		free(cmd->argv);
		free(cmd);
	}
	digitalWrite(NASH_TASK_LED_PIN, LOW);
	PRINT_PROMPT();
}

static void execute(struct command *cmd) {
	/* Find command */
	struct executable *exec = programs;
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
		back_to_prompt(cmd);
		return;
	}
	
	/* Executing */
	digitalWrite(NASH_TASK_LED_PIN, HIGH);
	current.command = cmd;
	current.worker = exec->worker;
	current.status = ALIVE;
	current.signal = 0;
	current.starttime = millis();
	current.inlen = 0;
}

static struct command * 
newcommand(char *line, size_t linelen) {
	struct command *cmd = malloc(sizeof(struct command));
	if (cmd == NULL) {
		ERRORLN("Out of memory");
		return NULL;
	}
	
	/* Copy buffer */
	memcpy(cmd->buff, line, linelen);
	cmd->buff[linelen] = NULL;
	
	/* Parse arguments */
	char *argv[NASH_MAX_ARGS];
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

static signal_t 
shell_readline(char **out, size_t *outlen) {
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

#if SERIAL_ECHO == ON
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
		
		/* Signals */
		case SIG_ESC:
		case SIG_INT:
		case SIG_EOF:
			return in;
		
		/* Normal characters */
		default:
			/* Line max */
			if (bufflen < NASH_LINE_SIZE) {
#if SERIAL_ECHO == ON
				WRITE(in);
#endif
				buff[bufflen] = in;
				bufflen++;
			}
	}
	return NOSIGNAL;
}


void 
nash_loop() {
	/* Process running task if any. */
	if (current.status == ALIVE) {
		/* Process is running */
		struct command *cmd = current.command;
		current.status = current.worker(cmd->argc, cmd->argv, &current);
		if (current.status != ALIVE) {
			/* Process has been terminated */
			back_to_prompt(current.command);
		}
	}
	
	/* read a line from input if any. */
	char *line;
	size_t linelen = 0;
	signal_t signal = shell_readline(&line, &linelen);
	
	/* New line */
	if (signal == SIG_NEWLINE) {
		if (current.status == ALIVE) {
			memcpy(current.input, line, linelen);
			current.inlen = linelen;
			current.input[linelen] = NULL;
		}
		else if (linelen == 0) {
			PRINT_PROMPT();
		}
		else {
			struct command * cmd = newcommand(line, linelen);	
			if (cmd == NULL) {
				PRINT_PROMPT();
				return NULL;
			}
			else {
				execute(cmd);
			}
		}
	}

	/* Signal */
	else if ((current.status == ALIVE) && signal) {
		current.signal = signal;
	}
}


void nash_init(struct executable *progs) {
	/* List of available commands */
	programs = progs;
	
	/* UART setup */
	Serial.begin(SERIAL_BAUDRATE);
	
	/* GPIO */
	pinMode(NASH_TASK_LED_PIN, OUTPUT);
	digitalWrite(NASH_TASK_LED_PIN, LOW);

	/* Initial Prompt */
	PRINT_PROMPT();
}

void nash_help() {
	struct executable *exec = programs;
	while(exec->name != NULL) {
		PRINT(exec->name);
		PRINT(" ");
		exec++;	
	}
	PRINTLN();
}


/* Report available memory */
#ifdef __arm__
	// should use uinstd.h to define sbrk but Due causes a conflict
	extern "C" char* sbrk(int incr);
#else 
	extern char *__brkval;
#endif

int freeMemory() {
	char top;
#ifdef __arm__
	return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	return &top - __brkval;
#else
	return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif
}

void nash_free() {
	PRINTLN(freeMemory());
}
