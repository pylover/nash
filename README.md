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
make upload PORT=/dev/ttyACM0
make screen
```

or

```bash
make clean demo upload screen
```

## Example

```C++
#include "nash.h"


Nash shell;


int8_t echo(Nash::Process *self) {
	for (int i = 1; i < self->argc; i++) {
		PRINT(self->argv[i]);
		if (self->argc - i > 1) {
			PRINT(" ");
		}
	}
	PRINTLN();
	return EXIT_SUCCESS;
}


int8_t sleep(Nash::Process *self) {
	if (self->signal == SIG_INT) {
		return EXIT_FAILURE;
	}
	unsigned long towait = atol(self->argv[1]) * 1000;
	unsigned long taken = millis() - self->starttime;
	if (taken < towait) {
		return ALIVE;
	}
	return EXIT_SUCCESS;
}


int8_t cat(Nash::Process *self) {
	if (self->signal == SIG_INT) {
		return EXIT_FAILURE;
	}
	else if (self->signal == SIG_EOF) {
		return EXIT_SUCCESS;
	}
	if (self->input != NULL) {
		PRINTLN(self->input);
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
```
