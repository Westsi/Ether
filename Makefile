CFILES:=$(shell find -L . -type f -name '*.c')
.PHONY: all clean clang

all: clean
	gcc $(CFILES) -Wall -o cweb -Iinclude/

clang: clean
	clang $(CFILES) -Wall -o cweb -Iinclude/ -fsanitize=address

clean:
	rm -rf ./cweb