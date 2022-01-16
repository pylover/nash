#include "nash.h"


int echo(char *args, byte argslen, signal_t sig) {
	return EXIT_SUCCESS;
};


static struct executable programs[] = {
	{"help", nash_help},
	{"echo", echo},
	{"ls", echo},
	{"cat", echo},
	{"sl", echo},
	{ NULL }
};

void setup() {
	nash_init(programs);
}

void loop() {
	nash_loop();	
}
