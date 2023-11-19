CFILES:=$(shell find -L . -type f -name '*.c')
.PHONY: all clean clang

all: clean
	gcc $(CFILES) -Wall -o ether -Iinclude/

clang: clean
	clang $(CFILES) -Wall -o ether -Iinclude/ -fsanitize=address

clean:
	rm -rf ./ether