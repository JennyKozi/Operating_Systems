all: selection_sort bubble_sort test test_merge

selection_sort: selection_sort.o
	gcc -o selection_sort selection_sort.o

selection_sort.o: selection_sort.c
	gcc -g3 -c -o selection_sort.o selection_sort.c

bubble_sort: bubble_sort.o
	gcc -o bubble_sort bubble_sort.o

bubble_sort.o: bubble_sort.c
	gcc -g3 -c -o bubble_sort.o bubble_sort.c

###################################################

test: test.o
	gcc -o test test.o

test.o: test.c
	gcc -g3 -c -o test.o test.c

test_merge: test_merge.o merge.o
	gcc -o test_merge test_merge.o merge.o

test_merge.o: test_merge.c
	gcc -g3 -c -o test_merge.o test_merge.c

merge.o: merge.c
	gcc -g3 -c -o merge.o merge.c

###################################################

run_test: test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./test

run: test_merge
	./test_merge

time: test_merge
	time ./test_merge

valgrind: test_merge
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./test_merge

gdb: test_merge
	gdb ./test_merge