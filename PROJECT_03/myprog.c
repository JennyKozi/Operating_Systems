#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "header.h"

#define NUM_ARGS_READER 10
#define NUM_ARGS_WRITER 12

volatile sig_atomic_t fl_s = 0;

// Handler for SIGUSR1
void handle_sigusr1(int signum) {
	fl_s = 1;
}

int main(int argc, char *argv[]) {

	if (argc != 2) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	int shmid = 50, count_readers = 0, count_writers = 0, index = 0, status;
	char *exec_file, *reader_args[NUM_ARGS_READER], *writer_args[NUM_ARGS_WRITER], temp_string[SIZE];
	pid_t *children_pids;
	FILE *fp;

	// Semaphores

	// Shared memory segment

	// Allocate memory
	for (int i = 0; i < NUM_ARGS_READER - 1; i++) {
		CHECK_MALLOC_NULL(reader_args[i] = malloc(SIZE * sizeof(char)));
	}
	for (int i = 0; i < NUM_ARGS_WRITER - 1; i ++) {
		CHECK_MALLOC_NULL(writer_args[i] = malloc(SIZE * sizeof(char)));
	}
	CHECK_MALLOC_NULL(children_pids = malloc(sizeof(pid_t)));

	// Open exec file
	exec_file = argv[1];
	fp = fopen(exec_file, "r");
	if (!fp) {
		printf("Can't open file!\n");
		exit(1);
	}

	// Read exec file
	while (fscanf(fp, "%s", temp_string) == 1) { // Scan program (reader / writer)

		children_pids = realloc(children_pids, (index + 1) * sizeof(pid_t));

		// Exec line to run a reader program
		if (strcmp("./reader", temp_string) == 0) {

			count_readers++;
			strcpy(reader_args[0], temp_string);
			for (int i = 1; i <= 6; i++) { // 1) "-f" 2) datafile 3) "-l" 4) recid 5) "-d" 6) time
				fscanf(fp, "%s", reader_args[i]);
			}
			strcpy(reader_args[7], "-s");
			snprintf(reader_args[8], sizeof(reader_args[8]), "%d", shmid);
			reader_args[9] = NULL;

			// Create a new process that will become a reader
			children_pids[index] = fork();
			if (children_pids[index] == -1) {
				perror("Failed to fork!\n");
				exit(1);
			}

			// Child -> reader
			if (children_pids[index] == 0) {
				signal(SIGUSR1, handle_sigusr1);
				while(!fl_s)
					pause(); // Wait for a signal to continue

				execvp("./reader", reader_args);
				perror("exec failure!\n");
				exit(1);
			}
		}

		// Exec line to run a writer program
		else {
			count_writers++;
			strcpy(writer_args[0], temp_string);

			for (int i = 1; i <= 8; i++) { // 1) "-f" 2) datafile 3) "-l" 4) recid 5) "-v" 6) value 7) "-d" 8) time
				fscanf(fp, "%s", writer_args[i]);
			}
			strcpy(writer_args[9], "-s");
			snprintf(writer_args[10], sizeof(writer_args[10]), "%d", shmid);
			writer_args[11] = NULL;

			// Create a new process that will become a writer
			children_pids[index] = fork();
			if (children_pids[index] == -1) {
				perror("Failed to fork!\n");
				exit(1);
			}

			// Child -> writer
			if (children_pids[index] == 0) {
				signal(SIGUSR1, handle_sigusr1);
				while(!fl_s)
					pause(); // Wait for a signal to continue

				execvp("./writer", writer_args);
				perror("exec failure!\n");
				exit(1);
			}
		}
		index++;
	}
	fclose(fp); // Close exec file

	// Send signal to children to continue
	for (int i = 0; i < index; i++) {
		kill(children_pids[i], SIGUSR1);
	}

	// Wait for readers and writers to finish
	for (int i = 0; i < count_readers + count_writers; i++) {
		wait(&status);
	}

	// Free memory
	for (int i = 0; i < NUM_ARGS_READER - 1; i++) {
		free(reader_args[i]);
	}
	for (int i = 0; i < NUM_ARGS_WRITER - 1; i++) {
		free(writer_args[i]);
	}
	free(children_pids);
	
    return 0;
}
