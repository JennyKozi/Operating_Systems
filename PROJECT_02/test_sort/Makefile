all: selection_sort bubble_sort test test_merge

selection_sort: selection_sort.o
	gcc -o selection_sort selection_sort.o

selection_sort.o: selection_sort.c
	gcc -c -o selection_sort.o selection_sort.c
	
bubble_sort: bubble_sort.o
	gcc -o bubble_sort bubble_sort.o

bubble_sort.o: bubble_sort.c
	gcc -c -o bubble_sort.o bubble_sort.c
	
###################################################

test: test.o
	gcc -o test test.o

test.o: test.c
	gcc -c -o test.o test.c
	
test_merge: test_merge.o merge_sort.o
	gcc -o test test.o merge_sort.o

test_merge.o: test.c
	gcc -c -o test.o test.c
	
merge_sort.o: merge_sort.c
	gcc -c -o merge_sort.o merge_sort.c

###################################################
	
run_test: test
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./test
	
run: test_merge
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --show-reachable=yes ./test_merge