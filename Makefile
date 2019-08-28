
ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

CFLAGS := -Ofast -march=native -frename-registers -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math
EMCCFLAGS := -O3 -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math

CC := gcc
EMCC := emcc

all: js cli cli-js

cli:
	$(CC) main.c -o jurischain $(CFLAGS)

dist:
	mkdir -p dist

js: dist
	$(EMCC) browser.c -o dist/jurischain.js -s WASM=0 $(EMCCFLAGS)

cli-js: dist
	$(EMCC) main.c -o dist/jurischain-cli.js $(EMCCFLAGS)

install:
	cp jurischain.h $(DESTDIR)$(PREFIX)/include/

clean:
	rm -rf ./jurischain
	rm -rf ./dist
	rm -rf *.gcda
