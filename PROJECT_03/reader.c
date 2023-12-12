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

	if (argc != 9) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	int i, recid, recid_min, recid_max, shmid;
	float time;
	bool flag_many_records = false;
	char *filename, temp_string[20];

	for (i = 1; i < argc; i += 2) {
		
		// Datafile
		if (strcmp("-f", argv[i]) == 0) {
			filename = argv[i + 1];
		}

		// Record(s)
		else if (strcmp("-l", argv[i]) == 0) {

			int size = strlen(argv[i + 1]);
			strcpy(temp_string, argv[i + 1]);

			// Many records
			if (temp_string[0] == '[') {

				flag_many_records = true;
				char min_s[10], max_s[10];

				// Find first record
				int j = 1, count = 0;
				while (isdigit(temp_string[j])) {
					min_s[count] = temp_string[j];
					count++;
					j++;
				}
				min_s[count] = '\0';
				recid_min = atoi(min_s);

				// Find last record 
				count = 0;
				j++;
				while (isdigit(temp_string[j])) {
					max_s[count] = temp_string[j];
					count++;
					j++;
				}
				max_s[count] = '\0';
				recid_max = atoi(max_s);
			}
			// One record
			else {
				recid = atoi(argv[i + 1]);
			}
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

//	printf("recid min: %d \nrecid max: %d \ntime: %f \nshm id: %d\n", recid_min, recid_max, time, shmid);

	exit(0);
}