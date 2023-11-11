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

static int Check_Int(char *);

int main(int argc, char *argv[]) {

	// MYSORT

	// Check number of arguments
	if (argc != 9) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	int i, j, k, total_records, rp, stat, size;
	long lSize;
	char *data_file, *sort1, *sort2;
	Record rec;
	struct stat buffer;

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
				perror("k must a positive integer!\n");
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
	lseek(rp, 0, SEEK_SET); // Set rp in the beginning of the file

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

	// Create k splitters with fork
	for (i = 0; i < k; i++) {
		splitters[i] = fork();
		if (splitters[i] == -1) {
			perror("Failed to fork!\n");
			exit(1);
		}

		// SPLITTERS
		if (splitters[i] == 0) {

			// Number of sorters this splitter has
			int numof_sorters = k - i;
			int load_sorter = splitters_numof_records[i] / k; // The first children (spliters) will get one extra record
			int remaining_load_sorters = splitters_numof_records[i] % k; // The first children (spliters) will get one extra record

			// Useful info for the sorters of this splitter
			pid_t *sorters;
			int *sorters_pointers, *sorters_numof_records, **sorters_pipes;
			CHECK_MALLOC_NULL(sorters = malloc(numof_sorters * sizeof(pid_t)));
			CHECK_MALLOC_NULL(sorters_pointers = malloc(numof_sorters * sizeof(int)));
			CHECK_MALLOC_NULL(sorters_numof_records = malloc(numof_sorters * sizeof(int)));
			CHECK_MALLOC_NULL(sorters_pipes = malloc(numof_sorters * sizeof(int *)));

			// Create pipes for splitters
			for (j = 0; j < numof_sorters; j++) {
				CHECK_MALLOC_NULL(sorters_pipes[j] = malloc(2 * sizeof(int)));
				if (pipe(sorters_pipes[j]) < 0) {
					perror("Error with pipe!\n");
					exit(1);
				}
			}

			// Number of records for the sorters
			for (j = 0; j < numof_sorters; j++) {
				if (remaining_load_sorters == 0) {
					sorters_numof_records[j] = load_sorter;
				}
				else {
					if (j < remaining_load_sorters)
						sorters_numof_records[j] = load_sorter + 1;
					else
						sorters_numof_records[j] = load_sorter;
				}
			}

			// File pointers for each sorter
			sorters_pointers[0] = splitters_pointers[i];
	
			for (j = 1; j < numof_sorters; j++) {
				sorters_pointers[j] = sorters_pointers[j - 1] + (sorters_numof_records[j - 1] * sizeof(rec));
			}

			// Create sorters of this splitter with fork
			for (j = 0; j < numof_sorters; j++) {

				sorters[j] = fork();
				if (sorters[j] == -1) {
					perror("Failed to fork!\n");
					exit(1);
				}

				// SORTERS
				if (sorters[j] == 0) {

					close(sorters_pipes[j][0]); // Close read end for sorter

					char pointer[10], num[10], pipe[10];
					snprintf(pointer, sizeof(pointer), "%d", sorters_pointers[j]);
					snprintf(num, sizeof(num), "%d", sorters_numof_records[j]);
					snprintf(pipe, sizeof(pipe), "%d", sorters_pipes[j][1]);

					// Execute a sorting algorithm
					if (j % 2 == 0) {
						execl("./quicksort", "./quicksort", data_file, pointer, num, pipe, (char *)NULL);
						perror("exec failure!\n");
						exit(1);
					}
					else {
						execl("./bubblesort", "./bubblesort", data_file, pointer, num, pipe, (char *)NULL);
						perror("exec failure!\n");
						exit(1);
					}
				}
				close(sorters_pipes[j][1]); // Close write end for splitter
			}
			// Parent process: SPLITTER
			// Read from pipes


			// Merge sorted records


			// Free memory allocated by splitter
			free(sorters);
			free(sorters_pointers);
			free(sorters_numof_records);
			for (j = 0; j < numof_sorters; j++) {
				free(sorters_pipes[j]);
			}
			free(sorters_pipes);
			exit(0); // End splitter process
		}
		close(splitters_pipes[i][1]); // Close write end for root
	}
	// Parent process: MYSORT
	// Read from pipes


	// Merge sorted records

	
	// Print sorted list


	close(rp); // Close file pointer for root

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

// Check that it is an int
static int Check_Int(char *string) {

	// Check for chars
	for (int i = 0; i < strlen(string); i++) {
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
