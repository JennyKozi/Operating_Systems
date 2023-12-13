#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "header.h"

void main (int argc, char *argv[]) {

	if (argc != 11) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	int i, recid, shmid, value;
	float time;
	bool flag_many_records = false;
	char *filename, temp_string[20];

	// Get arguments from command line
	for (i = 1; i < argc; i += 2) {
		
		// Datafile
		if (strcmp("-f", argv[i]) == 0) {
			filename = argv[i + 1];
		}

		// Record
		else if (strcmp("-l", argv[i]) == 0) {
			recid = atoi(argv[i + 1]);
		}

		// Value
		else if (strcmp("-v", argv[i]) == 0) {
			value = atoi(argv[i + 1]);
		}

		// Time
		else if (strcmp("-d", argv[i]) == 0) {
			time = atof(argv[i + 1]);
		}

		// Shared Memory Segment
		else if (strcmp("-s", argv[i]) == 0) {
			shmid = atoi(argv[i + 1]);
		}
	}

	// Binary datafile
	int rp, total_records, stat, file_size;
	Record rec;
	struct stat buffer;

	CHECK_FILE(rp = open(filename, O_RDONLY)); // Open file

	// Get number of records
	stat = fstat(rp, &buffer);
	if (stat != 0) {
		perror("Error reading file!\n");
		close(rp);
		exit(1);
	}
	close(rp); // Close file pointer
	file_size = buffer.st_size;
	total_records = (int)file_size / sizeof(rec); // Total records in the file

//	printf("recid: %d \nvalue: %d \ntime: %f \nshm id: %d\n", recid, value, time, shmid);

	//

	exit(0);
}
