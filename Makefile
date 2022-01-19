PORT = /dev/ttyACM0
BAUDRATE = 115200
PACKAGER = arduino
ARCH = avr
BOARD = uno
FQBN = $(PACKAGER):$(ARCH):$(BOARD)

.PHONY: env
env:
	arduino-cli core update-index
	arduino-cli core install $(PACKAGER):$(ARCH)

.PHONY: demo
demo:
	arduino-cli compile \
		--fqbn $(FQBN) \
		--library . \
		demo

.PHONY: upload
upload: demo
	arduino-cli upload -p $(PORT) --fqbn $(FQBN) demo

.PHONY: clean
clean:
	arduino-cli cache clean

.PHONY: screen
screen:
	screen $(PORT) $(BAUDRATE)

