#include "nash.h"


int echo(size_t argc, char **argv, struct process *self) {
	for (int i = 1; i < argc; i++) {
		PRINT(argv[i]);
		if (argc - i > 1) {
			PRINT(" ");
		}
	}
	PRINTLN();
	return EXIT_SUCCESS;
};


int sleep(size_t argc, char **argv, struct process *self) {
	if (self->signal == SIG_INT) {
		return EXIT_FAILURE;
	}
	unsigned long towait = atol(argv[1]) * 1000;
	unsigned long taken = millis() - self->starttime;
	if (taken < towait) {
		return ALIVE;
	}
	return EXIT_SUCCESS;
}

int cat(size_t argc, char **argv, struct process *self) {
	if (self->signal == SIG_INT) {
		return EXIT_FAILURE;
	}
	else if (self->signal == SIG_EOF) {
		return EXIT_SUCCESS;
	}
	if (self->inlen) {
		PRINTLN(self->input);
		self->inlen = 0;
	}
	return ALIVE;
}


static struct executable programs[] = {
	{"help", nash_help },
	{"echo", echo },
	{"sleep", sleep },
	{"cat", cat },
	{ NULL }
};

void setup() {
	nash_init(programs);
}

void loop() {
	nash_loop();	
}
