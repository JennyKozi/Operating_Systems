all: myprog reader writer

myprog: myprog.o
	gcc -o myprog myprog.o
	
myprog.o: myprog.c
	gcc -c -o myprog.o myprog.c

reader: reader.o
	gcc -o reader reader.o

reader.o: reader.c
	gcc -c -o reader.o reader.c

writer: writer.o
	gcc -o writer writer.o
	
writer.o: writer.c
	gcc -c -o writer.o writer.c
	
#######################################################

run: myprog
	./myprog -f accounts50.bin -r 10 -w 5

valgrind: myprog
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./myprog accounts50.bin

clean:
	rm -rf myprog myprog.o reader reader.o writer writer.o