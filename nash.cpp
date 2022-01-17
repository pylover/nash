
#include "nash.h"

static struct executable *programs;
static struct process current;


void execute(struct command *cmd) {

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
	digitalWrite(TASK_LED_PIN, HIGH);
	current.command = cmd;
	current.worker = exec->worker;
	current.status = ALIVE;
	current.signal = 0;
	current.starttime = millis();
	current.inlen = 0;
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

struct command * newcommand(char *line, size_t linelen) {
    struct command *cmd = malloc(sizeof(struct command));
    if (cmd == NULL) {
        ERRORLN("Out of memory");
        return NULL;
    }
    
    /* Copy & reset global buffer for future commands */
	memcpy(cmd->buff, line, linelen);
	cmd->buff[linelen] = NULL;
    
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
    return cmd;
}

void back_to_prompt(struct command *cmd) {
	if (cmd != NULL) {
		free(cmd->argv);
		free(cmd);
	}
	digitalWrite(TASK_LED_PIN, LOW);
	PRINT_PROMPT();
}


void nash_loop() {
	if (current.status == ALIVE) {
		/* Process is running */
		struct command *cmd = current.command;
		current.status = current.worker(cmd->argc, cmd->argv, &current);
		if (current.status != ALIVE) {
			/* Process terminated */
			back_to_prompt(current.command);
		}
	}
	
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
	programs = progs;
	pinMode(TASK_LED_PIN, OUTPUT);
	digitalWrite(TASK_LED_PIN, LOW);
	shell_init();
}
