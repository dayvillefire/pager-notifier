all: build

build:
	pio run -t upload && pio device monitor -b 115200
