CFILES:=$(shell find -L . -type f -name '*.c')
.PHONY: all clean

all: clean
	gcc $(CFILES) -Wall -o cweb -Iinclude/

clean:
	rm -rf ./cweb