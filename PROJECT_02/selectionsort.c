#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "header.h"

int main (int argc, char *argv[]) {

	// Check number of arguments
	if (argc != 5) {
		perror("Wrong number of arguments for selection sort!\n");
		exit(1);
	}

	int rp;
	CHECK_FILE(rp = open(argv[1], O_RDONLY));

	// File pointer
	int pointer = atoi(argv[2]);
	lseek(rp, pointer, SEEK_SET); // Set the file pointer

	// Number of records that will be sorted
	int num_records = atoi(argv[3]);

	// Pipe
	int pipe = atoi(argv[4]);

	// Read the file and sort the records


	close(rp);
	close(pipe);

	return 0;
}