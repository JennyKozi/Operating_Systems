#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
# include <sys/times.h>
#include "header.h"

int main (int argc, char *argv[]) {

	// Variables to count time
	double t1, t2, cpu_time;
	struct tms tb1, tb2;
	double ticspersec, ret_realtime, ret_cputime;
	int sum = 0, stop = -1;

	ticspersec = (double)sysconf(_SC_CLK_TCK);
	t1 = (double)times(&tb1);

	// Check number of arguments
	if (argc != 6) {
		perror("Wrong number of arguments for bubble sort!\n");
		exit(1);
	}

	// Open file
	int rp;
	CHECK_FILE(rp = open(argv[1], O_RDONLY));

	// Set file pointer
	int pointer = atoi(argv[2]);
	lseek(rp, pointer, SEEK_SET);

	// Number of records that will be sorted
	int num_records = atoi(argv[3]);

	// Pipe
	int pipe = atoi(argv[4]);

	// Root pid
	pid_t root_pid = atoi(argv[5]);

	// Read the file
	Record *array;
	Record rec;

	CHECK_MALLOC_NULL(array = malloc(num_records * sizeof(Record)));

	for (int i = 0; i < num_records; i++) {
		read(rp, &rec, sizeof(rec));
		array[i] = rec;
	}

	// Sort the records
	for (int i = 0; i < num_records; i++) {
		for (int j = num_records - 1; j >= i + 1; j--) {
			if (strcmp(array[j].LastName, array[j - 1].LastName) < 0) {
				rec = array[j];
				array[j] = array[j - 1];
				array[j - 1] = rec;
			}
			// Same last name
			else if (strcmp(array[j].LastName, array[j - 1].LastName) == 0) {
				if (strcmp(array[j].FirstName, array[j - 1].FirstName) < 0) {
					rec = array[j];
					array[j] = array[j - 1];
					array[j - 1] = rec;
				}
				// Same last name and first name
				else if (strcmp(array[j].FirstName, array[j - 1].FirstName) == 0) {
					if (array[j].custid < array[j - 1].custid) {
						rec = array[j];
						array[j] = array[j - 1];
						array[j - 1] = rec;
					}
				}
			}
		}
	}

	// Write the sorted array in the pipe
	for (int i = 0; i < num_records; i++) {
		write(pipe, &array[i].custid, sizeof(int));
		write(pipe, array[i].FirstName, sizeof(rec.FirstName));
		write(pipe, array[i].LastName, sizeof(rec.LastName));
		write(pipe, array[i].postcode, sizeof(rec.postcode));
	}
	write(pipe, &stop, sizeof(int));

	close(rp); // Close file pointer for sorter
	free(array); // Free memory

	// Calculate time
	t2 = (double)times(&tb2);
	cpu_time = (double)((tb2.tms_utime + tb2.tms_stime) - (tb1.tms_utime + tb1.tms_stime));
	ret_realtime = (double)((t2 - t1) / ticspersec);
	ret_cputime = (double)(cpu_time / ticspersec);

	write(pipe, &ret_realtime, sizeof(double));
	write(pipe, &ret_cputime, sizeof(double));

	close(pipe); // Close write end for sorter

	return 0;
}