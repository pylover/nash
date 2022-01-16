
#include "nash.h"

static struct executable *programs;
static struct process current;


void execute(struct command *cmd) {

	/* Find command */
	struct executable *exec = programs;
	while(exec->name != NULL) {
		if (strcmp(exec->name, cmd->name) == 0) {
			break;	
		}
		exec++;	
	}

	if (exec->name == NULL) {
		ERROR("Command '");
		ERROR(cmd->name);
		ERRORLN("' was not found.");
		PRINT_PROMPT();
		return;
	}
	
	/* Executing */
	current.command = cmd;
	current.worker = exec->worker;
	current.status = ALIVE;
	current.signal = 0;
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


void nash_loop() {
	if (current.status == ALIVE) {
		/* Process is running */
		current.status = current.worker(&current);
		if (current.status != ALIVE) {
			/* Process terminated */
			free(current.command);
			PRINT_PROMPT();
		}
		return;
	}

	struct command * cmd = shell_loop();
	if (cmd != NULL) {
		execute(cmd);
	}
}


void nash_init(struct executable *progs) {
	programs = progs;
	shell_init();
}
