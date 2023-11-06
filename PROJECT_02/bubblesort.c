#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"

int main (int argc, char *argv[]) { // argv[1] = fp

	// Check number of arguments
	if (argc != 3) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	// Number of records that will be sorted
	int num_records = atoi(argv[2]);
	

	return 0;
}