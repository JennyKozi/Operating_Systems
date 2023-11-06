#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
// #include <sys/wait.h>
#include "header.h"

static int Check_Int(char *);

int main(int argc, char *argv[]) {

	// MYSORT

	// Check number of arguments
	if (argc != 9) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	int i, k, numOfrecords, rp, status, size;
	long lSize;
	char *data_file, *sort1, *sort2;
	Record rec;
	struct stat buffer;

	int load_splitters = numOfrecords / k;
	int remaining_load = numOfrecords % k; // The first 'remaining_load' (e.g. 5) children / spliters will get one extra record
//	int total_sorters = (k * (k + 1)) / 2;

	// Check that the flags are correct
	for (i = 1; i <= 7; i += 2) {
		// File with data
		if (strcmp("-i", argv[i]) == 0) {
			data_file = argv[i + 1];
		}
		// k children
		else if (strcmp("-k", argv[i]) == 0) {
			if (Check_Int(argv[i + 1]) == 1) {
				k = atoi(argv[i + 1]);
			}
			else {
				perror("k must an integer!\n");
				exit(1);
			}
		}
		// Sorting program 1
		else if (strcmp("-e1", argv[i]) == 0) {
			sort1 = argv[i + 1];
		}
		// Sorting program 2
		else if (strcmp("-e2", argv[i]) == 0) {
			sort2 = argv[i + 1];
		}
		else {
			perror("Error with flags!\n");
			exit(1);
		}
	}

	// Open binary data file
	if ((rp = open(data_file, O_RDONLY)) == -1) {
		perror("Cannot open file!\n");
		exit(1);
	}
	// Get number of records
	status = fstat(rp, &buffer);
	if (status != 0) {
		printf("Error reading file!\n");
		close(rp);
		exit(1);
	}
	size = buffer.st_size;
	numOfrecords = (int)size / sizeof(rec);

	// Array for the splitters' ids
	pid_t *splitters;
	CHECK_MALLOC_NULL(splitters = malloc(k * sizeof(pid_t)));

	// Create k splitters with fork
	for (i = 0; i < k; i++) {
		splitters[i] = fork();
		if (splitters[i] == -1) {
			perror("Failed to fork!\n");
			exit(1);
		}
		// SPLITTERS
		if (splitters[i] == 0) {
			// Number of records for this splitter
			int num_records_splitter;


			// Number of sorters this splitter has
			int num_sorters = k - i;

			// Create the sorters of this splitter
			pid_t *sorters;
			CHECK_MALLOC_NULL(sorters = malloc(num_sorters * sizeof(pid_t)));

			// Create sorters with fork
			for (i = 0; i < num_sorters; i++) {
				sorters[i] = fork();
				if (sorters[i] == -1) {
					perror("Failed to fork!\n");
					exit(1);
				}
				// SORTERS
				if (sorters[i] == 0) {
					// Number of records for this sorter
					int num_records_sorter;


					//break;
				}
			}
			free(sorters);
		}
	}

	close(rp);
	free(splitters);

	return 0;
}

// Check that it is an int
static int Check_Int(char *string) {

	int i;
	// Check for chars
	for (i = 0; i < strlen(string); i++) {
		if (isalpha(string[i])) {
			return 0;
		}
	}
	// Check if num is float
	float float_num = atof(string);
	int num = float_num;
	float diff = float_num - (float)num;

	if (diff != 0.0) {
		return 0;
	}
	return 1;
}
