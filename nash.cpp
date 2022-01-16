
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
	current.command = cmd;
	current.worker = exec->worker;
	current.status = ALIVE;
	current.signal = 0;
	current.starttime = millis();
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
		struct command *cmd = current.command;
		current.status = current.worker(cmd->argc, cmd->argv, &current);
		if (current.status != ALIVE) {
			/* Process terminated */
			back_to_prompt(current.command);
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
