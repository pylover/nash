#include "nash.h"


Nash shell;


int echo(uint8_t argc, char **argv, Nash::Process *self) {
	for (int i = 1; i < argc; i++) {
		PRINT(argv[i]);
		if (argc - i > 1) {
			PRINT(" ");
		}
	}
	PRINTLN();
	return EXIT_SUCCESS;
};


int sleep(uint8_t argc, char **argv, Nash::Process *self) {
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


int cat(uint8_t argc, char **argv, Nash::Process *self) {
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


static Nash::Executable programs[] = {
	{"free", 0, 0, NULL, printFreeMemory},
	{"echo", 0, NASH_MAX_ARGS, "[STRING]...", echo},
	{"sleep", 1, 1, "NUMBER", sleep},
	{"cat", 0, 0, NULL, cat},
	{NULL}
};


void setup() {
	Serial.begin(115200);
	shell.init(programs);
}


void loop() {
	shell.loop();
}
