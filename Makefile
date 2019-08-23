all: 
	clang main.c -o sha3 -Ofast

clean:
	rm ./sha3
