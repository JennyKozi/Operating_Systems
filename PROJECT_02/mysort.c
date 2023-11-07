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

	int i, j, k, total_records, rp, stat, size, offset = 0;
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
	if ((rp = open(data_file, O_RDONLY)) == -1) {
		perror("Cannot open file!\n");
		exit(1);
	}

	// Get number of records
	stat = fstat(rp, &buffer);
	if (stat != 0) {
		perror("Error reading file!\n");
		close(rp);
		exit(1);
	}
	size = buffer.st_size;
	total_records = (int)size / sizeof(rec);
	lseek(rp, offset, SEEK_SET); // Set rp in the beginning of the file

	int load_splitter = total_records / k; // Number of records for one splitter
	int remaining_load_splitters = total_records % k; // The first children (spliters) will get one extra record

	// Useful info for the splitters
	pid_t *splitters;
	int *splitters_rp, *splitters_numof_records;
	CHECK_MALLOC_NULL(splitters = malloc(k * sizeof(pid_t)));
	CHECK_MALLOC_NULL(splitters_rp = malloc(k * sizeof(int)));
	CHECK_MALLOC_NULL(splitters_numof_records = malloc(k * sizeof(int)));

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
	for (i = 0; i < k; i++) {
		splitters_rp[i] = dup(rp); // Create a copy of the file pointer of the data file
		lseek(splitters_rp[i], i * splitters_numof_records[i] * sizeof(rec), SEEK_SET); // Set the file pointer of this splitter to the right byte
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
			int *sorters_rp, *sorters_numof_records;
			CHECK_MALLOC_NULL(sorters = malloc(numof_sorters * sizeof(pid_t)));
			CHECK_MALLOC_NULL(sorters_rp = malloc(numof_sorters * sizeof(int)));
			CHECK_MALLOC_NULL(sorters_numof_records = malloc(numof_sorters * sizeof(int)));

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
			for (j = 0; j < numof_sorters; j++) {
				sorters_rp[j] = dup(rp); // Create a copy of the file pointer of the data file
				lseek(sorters_rp[j], j * sorters_numof_records[j] * sizeof(rec), SEEK_SET); // Set the file pointer of this splitter to the right byte
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

					char str1[10], str2[10];
					snprintf(str1, sizeof(str1), "%d", sorters_rp[j]);
					snprintf(str2, sizeof(str2), "%d", sorters_numof_records[j]);

					// Execute a sorting algorithm
					if (j % 2 == 0) {
						execl("./quicksort", "./quicksort", str1, str2, (char *)NULL);
						perror("exec failure!\n");
						exit(1);
					}
					else {
						execl("./bubblesort", "./bubblesort", str1, str2, (char *)NULL);
						perror("exec failure!\n");
						exit(1);
					}
				}

				// Parent process: SPLITTER
				else {

				}
			}
			free(sorters);
			free(sorters_rp);
			free(sorters_numof_records);
		}

		// Parent process: MYSORT
		else {

		}
	}

	close(rp);
	free(splitters);
	free(splitters_rp);
	free(splitters_numof_records);

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
