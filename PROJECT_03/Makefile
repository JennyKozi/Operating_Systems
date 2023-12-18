all: myprog reader writer

myprog: myprog.o
	gcc -o myprog myprog.o -lrt
	
myprog.o: myprog.c
	gcc -c -o myprog.o myprog.c

reader: reader.o
	gcc -o reader reader.o -lrt

reader.o: reader.c
	gcc -c -o reader.o reader.c

writer: writer.o
	gcc -o writer writer.o -lrt
	
writer.o: writer.c
	gcc -c -o writer.o writer.c
	
#######################################################

run: myprog
	./myprog exec50.txt

valgrind: myprog
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./myprog exec50.txt

clean:
	rm -rf myprog myprog.o reader reader.o writer writer.o