#include "nash.h"


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
	if (argc != 2) {
		Nash::printUsage(self->executable, true);
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


int cat(uint8_t argc, char **argv, Nash::Process *self) {
	if (argc > 1) {
		Nash::printUsage(self->executable, true);
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


static Nash::Executable programs[] = {
	{"free", NULL, printFreeMemory},
	{"echo", "[STRING]...", echo},
	{"sleep", "NUMBER", sleep},
	{"cat", NULL, cat},
	{NULL}
};


Nash shell(programs);


void setup() {
	Serial.begin(115200);
	shell.init();
}


void loop() {
	shell.loop();
}
