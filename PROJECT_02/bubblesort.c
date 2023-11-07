#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

int main (int argc, char *argv[]) {

	// Check number of arguments
	if (argc != 3) {
		perror("Wrong number of arguments for bubblesort!\n");
		exit(1);
	}

	// File pointer
	int rp = atoi(argv[1]);

	// Number of records that will be sorted
	int num_records = atoi(argv[2]);

	// Read the file and sort the records
	

	return 0;
}