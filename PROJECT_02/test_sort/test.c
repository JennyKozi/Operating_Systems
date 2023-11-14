#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <sys/wait.h>
#include "header.h"

int main() {

	int i, k = 7, rp, size, total_records, stat, ret;
	Record rec;
	struct stat buffer;
	char *data_file = "voters50.bin";

	// Open binary data file
	CHECK_FILE(rp = open(data_file, O_RDONLY));

	// Get number of records
	stat = fstat(rp, &buffer);
	if (stat != 0) {
		perror("Error reading file!\n");
		close(rp);
		exit(1);
	}
	size = buffer.st_size;
	total_records = (int)size / sizeof(rec);
	lseek(rp, 0, SEEK_SET);

	int load_splitter = total_records / k; // Number of records for one splitter
	int remaining_load_splitters = total_records % k; // The first children (spliters) will get one extra record

	// Useful info for the splitters
	pid_t *splitters;
	int *splitters_pointers, *splitters_numof_records, **splitters_pipes;
	CHECK_MALLOC_NULL(splitters = malloc(k * sizeof(pid_t)));
	CHECK_MALLOC_NULL(splitters_pointers = malloc(k * sizeof(int)));
	CHECK_MALLOC_NULL(splitters_numof_records = malloc(k * sizeof(int)));
	CHECK_MALLOC_NULL(splitters_pipes = malloc(k * sizeof(int *)));

	// Create pipes for splitters
	for (i = 0; i < k; i++) {
		CHECK_MALLOC_NULL(splitters_pipes[i] = malloc(2 * sizeof(int)));
		if (pipe(splitters_pipes[i]) < 0) {
			perror("Error with pipe!\n");
			exit(1);
		}
	}
	
	// Number of records for the splitters
	for (i = 0; i < k; i++) {
		if (remaining_load_splitters == 0) {
			splitters_numof_records[i] = load_splitter;
		}
		else {
			if (i < remaining_load_splitters)
				splitters_numof_records[i] = load_splitter + 1;
			else
				splitters_numof_records[i] = load_splitter;
		}
	}

	// File pointers for each splitter
	splitters_pointers[0] = 0;
	
	for (i = 1; i < k; i++) {
		splitters_pointers[i] = splitters_pointers[i - 1] + (splitters_numof_records[i - 1] * sizeof(rec));
	}

	// Create k children
	for (i = 0; i < k; i++) {
		char file_pointer[10], num_recs[10], pipe[10];
		snprintf(file_pointer, sizeof(file_pointer), "%d", splitters_pointers[i]);
		snprintf(num_recs, sizeof(num_recs), "%d", splitters_numof_records[i]);
		snprintf(pipe, sizeof(pipe), "%d", splitters_pipes[i][1]);

		pid_t child = fork();
		if (child == -1) {
			perror("Failed to fork!\n");
			exit(1);
		}
		// CHILD: SORT
		else if (child == 0) {
			close(splitters_pipes[i][0]); // Close read end for child
			if (i % 2 == 0)
				execl("./selection_sort", "./selection_sort", data_file, file_pointer, num_recs, pipe, (char *)NULL);
			else
				execl("./bubble_sort", "./bubble_sort", data_file, file_pointer, num_recs, pipe, (char *)NULL);

			perror("exec failure!\n");
			exit(1);
		}
		close(splitters_pipes[i][1]); // Close write end for parent
	}

	// PARENT
	// Read data from pipes
	for (i = 0; i < k; i++) {
		while (read(splitters_pipes[i][0], &rec.custid, sizeof(int)) > 0 && rec.custid != -1) {
			read(splitters_pipes[i][0], rec.FirstName, sizeof(rec.FirstName));
			read(splitters_pipes[i][0], rec.LastName, sizeof(rec.LastName));
			read(splitters_pipes[i][0], rec.postcode, sizeof(rec.postcode));
			printf("%d %s %s %s\n", rec.custid, rec.LastName, rec.FirstName, rec.postcode);
		}
		read(splitters_pipes[i][0], &ret, sizeof(int));
		printf("\nRet: %d\n", ret);
		close(splitters_pipes[i][0]); // Close read end for parent
	}
	close(rp); // Close file pointer for parent

	// Free memory allocated by root
	free(splitters);
	free(splitters_pointers);
	free(splitters_numof_records);
	for (i = 0; i < k; i++) {
		free(splitters_pipes[i]);
	}
	free(splitters_pipes);

	return 0;
}
