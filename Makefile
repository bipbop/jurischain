all: js cli cli-js

cli:
	gcc main.c -o sha3 -Ofast -march=native -frename-registers -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math

js: 
	emcc browser.c -s WASM=0 -o sha3.js -O3  -frename-registers -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math

cli-js:
	emcc main.c -o sha3-cli.js -O3  -frename-registers -fassociative-math -freciprocal-math -fno-signed-zeros -fno-trapping-math


clean:
	rm ./sha3
	rm ./sha3.*
	rm main.gcda
