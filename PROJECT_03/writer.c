#include "header.h"

void main (int argc, char *argv[]) {

	if (argc != 11) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	int recid, shmid, value, rp, retval, err, index;
	float time;
	char *filename;
	shared_mem_seg *sh_mem;

	int pid = getpid();

	// Get arguments from command line
	for (int i = 1; i < argc; i += 2) {
		
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

	// Attach shared memory segment
	CHECK_CALL(sh_mem = shmat(shmid, (void *) 0, 0), -1);

	// Open file
	CHECK_CALL(rp = open(filename, O_RDONLY), -1); 

	// Set pointer to the right record (reach the balance int)
	lseek(rp, (recid * sizeof(Record) + sizeof(int) + 2 * NAME_SIZE * sizeof(char)), SEEK_SET);

	// Enter CS (new writer)
	sem_wait(&(sh_mem->sem_new_writer));
	for (int i = 0; i < ARRAY_SIZE; i++) {
		if (sh_mem->writers_pid[i] == 0) {
			sh_mem->writers_pid[i] = pid; // Add writer's pid to the array of reader's pids
			break;
		}
	}
	sem_post(&(sh_mem->sem_new_writer));
	// Exit CS (new writer)

	

	// Search for the rec in the arrays (if a process reads it or writes on it)


	// Enter CS (write)
	sem_wait(&(sh_mem->sem_readers_recs[index]));
	CHECK_CALL(write(rp, &value, sizeof(int)), -1);
	sem_post(&(sh_mem->sem_readers_recs[index]));
	// Exit CS (write)

	// Enter CS (finished writer)
	sem_wait(&(sh_mem->sem_finished_writer));
	sh_mem->total_writers++; // Increase number of writers
	sem_post(&(sh_mem->sem_finished_writer));
	// Exit CS (finished writer)
	
	// Close file
	CHECK_CALL(close(rp), -1);

	// Detach shared memory segment
	CHECK_CALL(err = shmdt((void *) sh_mem), -1);

	exit(0);
}
