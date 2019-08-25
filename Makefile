all: js cli

cli:
	gcc main.c -o sha3 -Ofast

js: 
	emcc main.c -o sha3.js -O3

clean:
	rm ./sha3
	rm ./sha3.*
