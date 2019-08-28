
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

CFLAGS := -Ofast -march=native -frename-registers -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math
EMCCFLAGS := -O3 -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math

CC := gcc
EMCC := emcc

all: js cli cli-js

cli:
	$(CC) main.c -o sha3 $(CFLAGS)

dist:
	mkdir -p dist

js: dist
	$(EMCC) browser.c -o dist/sha3.js -s WASM=0 $(EMCCFLAGS)

cli-js: dist
	$(EMCC) main.c -o dist/sha3-cli.js $(EMCCFLAGS)

install:
	cp juschain.h $(DESTDIR)$(PREFIX)/include/

clean:
	rm -rf ./sha3
	rm -rf ./dist
	rm -rf *.gcda
