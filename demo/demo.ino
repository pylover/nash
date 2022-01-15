#include "nash.h"


int echo(char *args, byte argslen, signal_t sig) {
	return EXIT_SUCCESS;
};


static struct command commands[] = {
	{"help", nash_help},
	{"echo", echo},
	{"ls", echo},
	{"cat", echo},
	{"sl", echo},
	{ NULL }
};

void setup() {
	nash_init(commands);
}

void loop() {
	nash_loop();	
}
