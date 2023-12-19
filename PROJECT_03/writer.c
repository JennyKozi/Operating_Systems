#include "header.h"

void main (int argc, char *argv[]) {

	if (argc != 11) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}
	printf("Writer says hello!\n");

	int i, recid, shmid, value, rp;
	float time;
	bool flag_many_records = false;
	char *filename;

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

	// Attach shared memory segment
	shared_mem_seg *sh_mem;
	int retval, id, err;

	sh_mem = shmat(shmid, (void *) 0, 0);
	if (sh_mem == (void *) -1) {
		perror("Can't attach shared memory segment!\n");
		exit(1);
	}

	// Enter CS (First CS for int total_writers)
	sem_wait(&(sh_mem->sem_new_writer));
	sh_mem->total_writers++;
	sem_post(&(sh_mem->sem_new_writer));
	// Exit CS

	CHECK_CALL(rp = open(filename, O_RDONLY), -1); // Open file

	// Set pointer to the right record (reach the balance int)
	lseek(rp, (recid * sizeof(Record) + sizeof(int) + 2 * NAME_SIZE * sizeof(char)), SEEK_SET);
	

	// Enter CS (Second CS to change data in the file)

	//CHECK_CALL(write(rp, &value, sizeof(int)), -1);

	// Exit CS

	int pid = getpid();
	printf("Writer %d\n", pid);

	CHECK_CALL(close(rp), -1); // Close file

	// Detach shared memory segment
	CHECK_CALL(err = shmdt((void *) sh_mem), -1);

	exit(0);
}
