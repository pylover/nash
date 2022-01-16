#include "nash.h"


int echo(size_t argc, char **argv, signal_t signal) {
	for (int i = 1; i < argc; i++) {
		PRINT(argv[i]);
		if (argc - i > 1) {
			PRINT(" ");
		}
	}
	PRINTLN();
	return EXIT_SUCCESS;
};


static struct executable programs[] = {
	{"help", nash_help },
	{"echo", echo },
	{ NULL }
};

void setup() {
	nash_init(programs);
}

void loop() {
	nash_loop();	
}
