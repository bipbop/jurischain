
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

js: 
	$(EMCC) browser.c -o sha3.js -s WASM=0 $(EMCCFLAGS)

cli-js:
	$(EMCC) main.c -o sha3-cli.js $(EMCCFLAGS)

install:
	cp sha3pow.h $(DESTDIR)$(PREFIX)/include/

clean:
	rm ./sha3
	rm ./sha3.*
	rm main.gcda
