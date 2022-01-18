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
	if (argc != 2) {
		ERRORLN("Invalid number of arguments");
		return EXIT_FAILURE;
	}
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
	if (argc > 1) {
		ERRORLN("Invalid number of arguments");
		nash_print_usage(self->executable);
		return EXIT_FAILURE;
	}
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
	{"help", NULL, nash_help},
	{"free", NULL, nash_free},
	{"echo", "[STRING]...", echo},
	{"sleep", "NUMBER", sleep},
	{"cat", NULL, cat},
	{NULL, NULL, NULL}
};


void setup() {
	nash_init(programs);
}


void loop() {
	nash_loop();	
}
