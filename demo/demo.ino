#include "nash.h"


#ifdef __arm__
	// should use uinstd.h to define sbrk but Due causes a conflict
	extern "C" char* sbrk(int incr);
#else 
	extern char *__brkval;
#endif

int freeMemory() {
	char top;
#ifdef __arm__
	return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
	return &top - __brkval;
#else
	return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif
}

int free(size_t argc, char **argv, struct process *self) {
	PRINTLN(freeMemory());
}


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
	{"free", free },
	{ NULL }
};

void setup() {
	nash_init(programs);
}

void loop() {
	nash_loop();	
}
