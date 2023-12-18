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

	// Attach shared memory segment
	shared_mem_seg *sh_mem;
	int retval, id, err;

	sh_mem = shmat(shmid, (void *) 0, 0);
	if (sh_mem == (void *) -1) {
		perror("Can't attach shared memory segment!\n");
		exit(1);
	}

	//	printf("recid: %d \nvalue: %d \ntime: %f \nshm id: %d\n", recid, value, time, shmid);

	// Detach shared memory segment
	err = shmdt((void *) sh_mem);
	if (err == -1) {
		perror ("Writer detaches shared memory segment.\n");
	}

	exit(0);
}
