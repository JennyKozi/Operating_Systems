#include "header.h"

void main (int argc, char *argv[]) {

	if (argc != 9) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	int recid, recid_min, recid_max, shmid, rp, retval, err, index;
	float time;
	char *filename, temp_string[NAME_SIZE];
	bool flag_many_records = false;
	shared_mem_seg *sh_mem;

	int pid = getpid();

	// Get arguments from command line
	for (int i = 1; i < argc; i += 2) {
		
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
	CHECK_CALL(sh_mem = shmat(shmid, (void *) 0, 0), (void *) -1);

	// Open file
	CHECK_CALL(rp = open(filename, O_RDONLY), -1);

	// Set pointer to the right record
	lseek(rp, recid * sizeof(Record), SEEK_SET);

	// Enter CS (new reader)
	sem_wait(&(sh_mem->sem_new_reader));
	for (int i = 0; i < ARRAY_SIZE; i++) {
		if (sh_mem->readers_pid[i] == 0) {
			sh_mem->readers_pid[i] = pid; // Add readers's pid to the array of reader's pids
			break;
		}
	}
	sem_post(&(sh_mem->sem_new_reader));
	// Exit CS (new reader)

	// Enter CS (insert rec id in the array)
	sem_wait(&(sh_mem->mutex));
	for (int i = 0; i < ARRAY_SIZE; i++) {
		
	}
	sem_post(&(sh_mem->mutex));
	// Exit CS (insert rec id in the array)

	

	// Search for rec in the arrays (if a process reads it or writes on it)
	

	// Enter CS (read)
	sem_wait(&(sh_mem->sem_readers_recs[index]));

	sem_post(&(sh_mem->sem_readers_recs[index]));
	// Exit CS (read)

	// Enter CS (finished reader)
	sem_wait(&(sh_mem->sem_finished_reader));
	sh_mem->total_readers++; // Increase number of readers
	sem_post(&(sh_mem->sem_finished_reader));
	// Exit CS (finished reader)

	printf("Reader %d\n", pid);

	// Close file
	CHECK_CALL(close(rp), -1);

	// Detach shared memory segment
	CHECK_CALL(err = shmdt((void *) sh_mem), -1);

	exit(0);
}
