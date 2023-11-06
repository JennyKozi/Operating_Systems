#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
//#include <sys/wait.h>

// Check if the memory is allocated and malloc didn't fail
#define CHECK_MALLOC_NULL(p)  \
if ((p) == NULL) {  \
	printf("Cannot allocate memory!\n"); \
	exit(1);  \
};

int Check_Int(char *);

int main(int argc, char *argv[]) {

	// Check number of arguments
	if (argc != 9) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}
	
	// Check that the flags are correct
	int i, k;
	char *data_file, *sort1, *sort2;

	for (i = 1; i <= 7; i += 2) {
		// File with data
		if (strcmp("-i", argv[i]) == 0) {
			data_file = argv[i];
		}
		// k children
		else if (strcmp("-k", argv[i]) == 0) {
			if (Check_Int(argv[i]) == 1) {
				k = atoi(argv[i]);
			}
			else {
				perror("k must an integer!");
				exit(1);
			}
		}
		// Sorting program 1
		else if (strcmp("-e1", argv[i]) == 0) {
			sort1 = argv[i];
		}
		// Sorting program 2
		else if (strcmp("-e2", argv[i]) == 0) {
			sort2 = argv[i];
		}
		else {
			perror("Error with flags!\n");
			exit(1);
		}
	}

	// Open data file
	FILE *fpb = fopen(argv[1], "rb");
	if (fpb == NULL) {
		perror("Cannot open binary file\n");
		exit(1);
	}

	// Array for the children's ids
	pid_t *childpid;
	CHECK_MALLOC_NULL(childpid = malloc(k * sizeof(pid_t)));

	// Create k children with fork
	for (i = 0; i < k; i++) {
		childpid[i] = fork();
		if (childpid[i] == -1) {
			perror("Failed to fork!\n");
			exit(1);
		}
		// Children must not create other children here
		if (childpid[i] == 0) {
			break;
		}
	}

	fclose(fpb);
	free(childpid);

	return 0;
}

// Check that it is an int
int Check_Int(char *string) {

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
