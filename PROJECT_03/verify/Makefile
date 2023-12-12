all: verify

verify: verify.c
	gcc verify.c -o verify
	
run: verify
	./verify accounts50.bin

clean:
	rm -rf verify verify.o
