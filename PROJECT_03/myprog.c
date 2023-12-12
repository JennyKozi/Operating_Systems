#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <sys/wait.h>
#include "header.h"

#define READERS_NUM 10
#define WRITERS_NUM 4

static int Check_Int(char *);

int main(int argc, char *argv[]) {

	if (argc != 3 || argc != 5 || argc != 7) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	int i, numof_readers, numof_writers;
	bool flag_readers = false, flag_writers = false;
	char *filename;

	for (i = 1; i < argc; i += 2) {
		
		// Datafile
		if (strcmp("-f", argv[i]) == 0) {
			filename = argv[i + 1];
		}

		// Readers
		else if (strcmp("-r", argv[i]) == 0) {
			if (Check_Int(argv[i + 1]) == 1) {
				numof_readers = atoi(argv[i + 1]);
				flag_readers = true; // Number of readers was given from the user
			}
			else { // numof_readers is float or string
				perror("Number of readers is not a positive integer!\n");
				exit(1);
			}
		}

		// Writers
		else if (strcmp("-w", argv[i]) == 0) {
			if (Check_Int(argv[i + 1]) == 1) {
				numof_writers = atoi(argv[i + 1]);
				flag_writers = true; // Number of writers was given from the user
			}
			else { // numof_writers is float or string
				perror("Number of writers is not a positive integer!\n");
				exit(1);
			}
		}
	}

	// User didn't give number of readers and/or number of writers: we use the default numbers
	if (flag_readers == false) {
		numof_readers = READERS_NUM;
	}
	if (flag_writers == false) {
		numof_writers = WRITERS_NUM;
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