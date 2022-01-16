
#include "nash.h"

static struct programs *programs;
static struct process current;


// void execute(struct process *p) {
// 
// 	/* Find command */
// 	struct command *cmd;
// 	byte counter = 0;
// 	do {
// 		cmd = &commands[counter++];	
// 		if (strcmp(cmdname, cmd->name) == 0) {
// 			break;	
// 		}
// 	} while(cmd->name != NULL);
// 
// 	if (cmd->name == NULL) {
// 		ERROR("Command '");
// 		ERROR(cmdname);
// 		ERRORLN("' was not found.");
// 		p->status = -1;
// 		return;
// 	}
// 	
// 	/* Executing */
// 	p->command = cmd;	
// }


// void nash_help() {
// 	PRINT("Available Commands: ");
// 	int counter = 0;
// 	while (true) {
// 		struct command cmd = commands[counter++];
// 		if (cmd.name == NULL) {
// 			break;
// 		}
// 		PRINT(cmd.name);
// 		PRINT(" ");
// 	}
// 	PRINTLN();
// }


void nash_loop() {
	struct command * cmd = shell_loop();
	
	if (cmd != NULL) {
		PRINTLN(cmd->name);
		// TODO: execute
		free(cmd);
		PRINT_PROMPT();
	}
}


void nash_init(struct executable *programs) {
	programs = programs;
	shell_init();
}
