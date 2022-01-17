# nash
A simple *nix-like shell for Arduino.


```bash
Sketch uses 4400 bytes (13%) of program storage space. Maximum is 32256 bytes.
Global variables use 490 bytes (23%) of dynamic memory, leaving 1558 bytes for local variables. Maximum is 2048 bytes.
```

## Compile and run demo

You need an arduino uno connected to your Ubuntu machine.

Install screen using `apt install screen`.
Install [arduino-cli](https://github.com/arduino/arduino-cli), then:

### Initialize your arduino-cli environment:

```bash
make env
```

### Compile and upload the sketch

```bash
make demo 
make upload 
make screen
```

or

```bash
make clean demo upload scrren
```

## Example

```C++
#include <nash.h>

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
```
