#include "header.h"

#define NUM_ARGS_READER 10
#define NUM_ARGS_WRITER 12
# define SEGMENTPERM 0666

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

	int count_readers = 0, count_writers = 0, index = 0, status;
	char *exec_file, *reader_args[NUM_ARGS_READER], *writer_args[NUM_ARGS_WRITER], temp_string[SIZE];
	pid_t *children_pids;
	FILE *fp;

	// Create shared memory segment
	sem_t *sp;
	shared_mem_seg *sh_mem;
	int retval, shmid, err;

	shmid = shmget(IPC_PRIVATE, sizeof(shared_mem_seg), SEGMENTPERM);
	if (shmid == (void *) -1) {
		perror("Can't create shared memory segment!\n");
		exit(1);
	}
	else {
		printf("Allocated %d\n", shmid);
	}

	// Attach the segment
	sh_mem = shmat(shmid, (void *) 0, 0);
	if (sh_mem == (void *) -1) {
		perror("Can't attach shared memory segment!\n");
		exit(1);
	}

	// Initialize data of shared memory segment
	(*sh_mem).num_readers = 0;
	(*sh_mem).num_writers = 0;
	(*sh_mem).num_recs_processed = 0;

	// Initialize the semaphore
/*
	retval = sem_init(sp, 1, 2);
	if (retval != 0) {
		perror("Couldn't initialize the semaphore!\n");
		exit(1);
	}
*/

	// Allocate memory
	for (int i = 0; i < NUM_ARGS_READER - 1; i++) {
		CHECK_MALLOC_NULL(reader_args[i] = malloc(SIZE * sizeof(char)));
	}
	for (int i = 0; i < NUM_ARGS_WRITER - 1; i++) {
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

//	sem_destroy(sp); // Destroy semaphore

	// Destroy shared memory segment
	err = shmctl(shmid, IPC_RMID, 0);
	if (err == -1)
		perror("Can't remove shared memory segment!\n");
	else
		printf("Removed shared memory segment %d\n", err);

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
