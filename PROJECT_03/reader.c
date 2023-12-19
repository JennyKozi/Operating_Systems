#include "header.h"

void main (int argc, char *argv[]) {

	if (argc != 9) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}
	printf("Reader says hi!\n");
	int i, recid, recid_min, recid_max, shmid, rp;
	float time;
	bool flag_many_records = false;
	char *filename, temp_string[NAME_SIZE];

	// Get arguments from command line
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

	// Attach shared memory segment
	shared_mem_seg *sh_mem;
	int retval, id, err;

	CHECK_CALL(sh_mem = shmat(shmid, (void *) 0, 0), (void *) -1);

	// Enter CS (First CS for int total_readers)
	sem_wait(&(sh_mem->sem_new_reader));
	sh_mem->total_readers++;
	sem_post(&(sh_mem->sem_new_reader));
	// Exit CS

	CHECK_CALL(rp = open(filename, O_RDONLY), -1); // Open file

	// Set pointer to the right record
	lseek(rp, recid * sizeof(Record), SEEK_SET);


	// Enter CS (Second CS to read data from the file)

	// Exit CS

	int pid = getpid();
	printf("Reader %d\n", pid);

	CHECK_CALL(close(rp), -1); // Close file

	// Detach shared memory segment
	CHECK_CALL(err = shmdt((void *) sh_mem), -1);

	exit(0);
}
