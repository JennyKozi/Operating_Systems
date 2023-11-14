all: mysort merge_sort selectionsort bubblesort

mysort: mysort.o merge_sort.o
	gcc -o mysort mysort.o merge_sort.o

mysort.o: mysort.c
	gcc -c -o mysort.o mysort.c

merge_sort.o: merge_sort.c
	gcc -c -o merge_sort.o merge_sort.c

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