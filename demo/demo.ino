#include "nash.h"


int echo(struct process *self) {
	PRINTLN(self->command->args);
	return EXIT_SUCCESS;
};


static struct executable programs[] = {
	{ "help", nash_help },
	{ "echo", echo },
	{ NULL }
};

void setup() {
	nash_init(programs);
}

void loop() {
	nash_loop();	
}
