all: mysort selectionsort bubblesort

mysort: mysort.o
	gcc -o mysort mysort.o

mysort.o: mysort.c
	gcc -c -o mysort.o mysort.c

selectionsort: selectionsort.o
	gcc -o selectionsort selectionsort.o

selectionsort.o: selectionsort.c
	gcc -c -o selectionsort.o selectionsort.c
	
bubblesort: bubblesort.o
	gcc -o bubblesort bubblesort.o
	
bubblesort.o: bubblesort.c
	gcc -c -o bubblesort.o bubblesort.c

clean:
	rm -rf mysort mysort.o selectionsort selectionsort.o bubblesort bubblesort.o