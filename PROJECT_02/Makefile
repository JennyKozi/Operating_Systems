all: mysort quicksort bubblesort

mysort: mysort.o
	gcc -o mysort mysort.o

mysort.o: mysort.c
	gcc -c -o mysort.o mysort.c

quicksort: quicksort.o
	gcc -o quicksort quicksort.o

quicksort.o: quicksort.c
	gcc -c -o quicksort.o quicksort.c
	
bubblesort: bubblesort.o
	gcc -o bubblesort bubblesort.o
	
bubblesort.o: bubblesort.c
	gcc -c -o bubblesort.o bubblesort.c

clean:
	rm -rf mysort mysort.o quicksort quicksort.o bubblesort bubblesort.o