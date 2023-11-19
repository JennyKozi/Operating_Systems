#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "header.h"

void merge(Record **, int, int, int);

int main() {

	int i, k = 4, rp, size, total_records, stat, status;
	Record rec;
	struct stat buffer;
	char *data_file = "voters50000.bin";
	pid_t root_pid = getpid();
	char *sort1 = "bubble_sort", *sort2 = "selection_sort", *prog1, *prog2;

	// Preparing the strings for the execution of the programs
	CHECK_MALLOC_NULL(prog1 = malloc((strlen(sort1) + 3) * sizeof(char)));
	CHECK_MALLOC_NULL(prog2 = malloc((strlen(sort2) + 3) * sizeof(char)));
	strcpy(prog1, "./");
	strcat(prog1, sort1);
	strcpy(prog2, "./");
	strcat(prog2, sort2);

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
	double **sorters_time;
	CHECK_MALLOC_NULL(splitters = malloc(k * sizeof(pid_t)));
	CHECK_MALLOC_NULL(splitters_pointers = malloc(k * sizeof(int)));
	CHECK_MALLOC_NULL(splitters_numof_records = malloc(k * sizeof(int)));
	CHECK_MALLOC_NULL(splitters_pipes = malloc(k * sizeof(int *)));
	CHECK_MALLOC_NULL(sorters_time = malloc(k * sizeof(double *)));

	// Matrix for real time and cpu time of each sorter
	for (i = 0; i < k; i++) {
		CHECK_MALLOC_NULL(sorters_time[i] = malloc(2 * sizeof(double)));
	}

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
		char file_pointer[10], num_recs[10], pipe[10], pid[10];
		snprintf(file_pointer, sizeof(file_pointer), "%d", splitters_pointers[i]);
		snprintf(num_recs, sizeof(num_recs), "%d", splitters_numof_records[i]);
		snprintf(pipe, sizeof(pipe), "%d", splitters_pipes[i][1]);
		snprintf(pid, sizeof(pid), "%d", root_pid);

		splitters[i] = fork();
		if (splitters[i] == -1) {
			perror("Failed to fork!\n");
			exit(1);
		}
		// CHILD: SORT
		else if (splitters[i] == 0) {
			close(splitters_pipes[i][0]); // Close read end for child
			if (i % 2 == 0)
				execl(prog1, prog1, data_file, file_pointer, num_recs, pipe, pid, (char *)NULL);
			else
				execl(prog2, prog2, data_file, file_pointer, num_recs, pipe, pid, (char *)NULL);

			perror("exec failure!\n");
			exit(1);
		}
		close(splitters_pipes[i][1]); // Close write end for parent
	}

	// PARENT
	Record **splitters_results;
	CHECK_MALLOC_NULL(splitters_results = malloc(k * sizeof(Record *)));
	for (i = 0; i < k; i++) {
		CHECK_MALLOC_NULL(splitters_results[i] = malloc(splitters_numof_records[i] * sizeof(Record)));
	}

	// Read data from pipes
	for (i = 0; i < k; i++) {
		int count = 0;
		waitpid(splitters[i], &status, WNOHANG);
		while (read(splitters_pipes[i][0], &rec.custid, sizeof(int)) > 0 && rec.custid != -1) {
			read(splitters_pipes[i][0], rec.FirstName, sizeof(rec.FirstName));
			read(splitters_pipes[i][0], rec.LastName, sizeof(rec.LastName));
			read(splitters_pipes[i][0], rec.postcode, sizeof(rec.postcode));
			splitters_results[i][count++] = rec;
		}
		// Read time from child
		read(splitters_pipes[i][0], &sorters_time[i][0], sizeof(double)); // Real time
		read(splitters_pipes[i][0], &sorters_time[i][1], sizeof(double)); // CPU time
		close(splitters_pipes[i][0]); // Close read end for parent
	}

	// Merge results
	Record *final_result;
	int prev_size = 0;
	int current_size = splitters_numof_records[0];
	CHECK_MALLOC_NULL(final_result = malloc(current_size * sizeof(Record)));
	for (i = 0; i < current_size; i++) {
		final_result[i] = splitters_results[0][i];
	}

	for (i = 1; i < k; i++) {
		int count = 0;
		prev_size = current_size;
		current_size += splitters_numof_records[i];
		final_result = realloc(final_result, current_size * sizeof(Record));
		for (int j = prev_size; j < current_size; j++) {
			final_result[j] = splitters_results[i][count++];
		}
		merge(&final_result, 0, prev_size - 1,  current_size - 1);
	}

	// Print final sorted list
	for (i = 0; i < total_records; i++) {
		printf("%s %s %d %s\n", final_result[i].LastName, final_result[i].FirstName, final_result[i].custid, final_result[i].postcode);
	}
	printf("\n");

	close(rp); // Close file pointer for parent

	// Print time of sorters
	for (i = 0; i < k; i++) {
		printf("Sorter %d: Real time: %f, CPU time: %f\n", i, sorters_time[i][0], sorters_time[i][1]);
	}

	// Free memory allocated by root
	free(prog1);
	free(prog2);
	free(splitters);
	free(splitters_pointers);
	free(splitters_numof_records);
	for (i = 0; i < k; i++) {
		free(splitters_pipes[i]);
	}
	free(splitters_pipes);
	for (i = 0; i < k; i++) {
		free(splitters_results[i]);
	}
	free(splitters_results);
	for (i = 0; i < k; i++) {
		free(sorters_time[i]);
	}
	free(sorters_time);
	free(final_result);

	return 0;
}
