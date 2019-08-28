all: js cli

cli:
	gcc main.c -o sha3 -Ofast

js: 
	emcc browser.c -s WASM=0 -o sha3.js -O3

clean:
	rm ./sha3
	rm ./sha3.*
