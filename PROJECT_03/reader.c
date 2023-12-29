#include "header.h"

void main (int argc, char *argv[]) {

	if (argc != 9) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	// Variables to count time
	double t1, t2, realtime, tics_per_sec;
	struct tms tb1, tb2;
	tics_per_sec = (double)sysconf(_SC_CLK_TCK);
	t1 = (double)times(&tb1);

	int recid, recid_min, recid_max, shmid, rp, retval, err, rec_index, pid_index, num_recs = 1, last_writer, time;
	char *filename, temp_string[NAME_SIZE];
	bool flag_many_records = false;
	shared_mem_seg *sh_mem;
	Record rec;
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
				num_recs = recid_max - recid_min + 1;
			}
			// One record
			else {
				recid = atoi(argv[i + 1]);
			}
		}

		// Time
		else if (strcmp("-d", argv[i]) == 0) {
			time = atoi(argv[i + 1]);
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
			pid_index = i;
			break;
		}
	}
	sem_post(&(sh_mem->sem_new_reader));
	// Exit CS (new reader)

	// Enter CS (insert rec id in the array)
	sem_wait(&(sh_mem->mutex));
	rec_index = sh_mem->total_readers; // Very important: variable rec_index indicates the index for the array of records for the readers
	last_writer = sh_mem->total_writers; // The last writer that arrived before this process
	sh_mem->count_processes++;
	sh_mem->total_readers++; // Increase number of readers
	sh_mem->readers_recs[rec_index][0] = recid; // Insert the record id of this reader in the array
	sem_post(&(sh_mem->mutex));
	// Exit CS (insert rec id in the array)

	// Enter CS (Read)
	sem_wait(&(sh_mem->sem_readers_recs[rec_index]));
	// Enter CS (Search records)
	sem_wait(&(sh_mem->mutex));

	// There is one or more writers who write on this record
	for (int i = 0; i < last_writer; i++) {
		if (sh_mem->sem_writers_recs[i] == recid) {
			sem_post(&(sh_mem->mutex)); // Leave mutex, don't hold back the other processes
			sem_wait(&(sh_mem->sem_writers_recs[i])); // Reader is suspended until writers (who came before) finish
			sem_wait(&(sh_mem->mutex));
		}
	}
	sem_post(&(sh_mem->mutex));
	// Exit CS (Search records)

	read(rp, &rec, sizeof(Record));
	sem_post(&(sh_mem->sem_readers_recs[rec_index]));
	// Exit CS (Read)

	// Enter CS (Remove id of this rec from the array)
	sem_wait(&(sh_mem->mutex));
	sh_mem->readers_recs[rec_index][0] = 0;
	sem_post(&(sh_mem->mutex));
	// Exit CS (Remove id of this rec from the array)

	// Close file
	CHECK_CALL(close(rp), -1);

	// Enter CS (increase records processed)
	sem_wait(&(sh_mem->sem_sum));
	sh_mem->total_recs_processed += num_recs; // Increase number of records that have been processed
	sem_wait(&(sh_mem->sem_sum));
	// Exit CS (increase records processed)

	// Enter CS (Remove readers's pid from the array of readers' pids)
	sem_wait(&(sh_mem->sem_new_reader));
	sh_mem->readers_pid[pid_index] = 0;
	sem_post(&(sh_mem->sem_new_reader));
	// Exit CS (Remove readers's pid from the array of readers' pids)

	printf("Reader %d\n", pid);

	// Calculate time
	t2 = (double)times(&tb2);
	realtime = (double)((t2 - t1) / tics_per_sec);
	sh_mem->time_readers[rec_index] = realtime;

	// Detach shared memory segment
	CHECK_CALL(err = shmdt((void *) sh_mem), -1);

	exit(0);
}
