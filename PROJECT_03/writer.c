#include "header.h"

void main (int argc, char *argv[]) {

	if (argc != 11) {
		perror("Wrong number of arguments!\n");
		exit(1);
	}

	// Variables to count time
	double t1, t2, realtime, tics_per_sec;
	struct tms tb1, tb2;
	tics_per_sec = (double)sysconf(_SC_CLK_TCK);
	t1 = (double)times(&tb1);

	int recid, shmid, value, rp, retval, err, rec_index, pid_index, num_recs = 1, last_reader, time;
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
			time = atoi(argv[i + 1]);
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

	// Set pointer to the right record (reach the int balance)
	lseek(rp, (recid * sizeof(Record) + sizeof(int) + 2 * NAME_SIZE * sizeof(char)), SEEK_SET);

	// Enter CS (New writer)
	sem_wait(&(sh_mem->sem_new_writer));
	for (int i = 0; i < ARRAY_SIZE; i++) {
		if (sh_mem->writers_pid[i] == 0) {
			sh_mem->writers_pid[i] = pid; // Add writer's pid to the array of reader's pids
			pid_index = i;
			break;
		}
	}
	sem_post(&(sh_mem->sem_new_writer));
	// Exit CS (New writer)

	// Enter CS (Insert rec id in the array)
	sem_wait(&(sh_mem->mutex));
	rec_index = sh_mem->total_writers; // Very important: variable rec_index indicates the index for the array of records for the writers
	last_reader = sh_mem->total_readers; // The last reader that arrived before this process
	sh_mem->count_processes++;
	sh_mem->total_writers++; // Increase number of writers
	sh_mem->writers_recs[rec_index] = recid; // Insert the record id of this reader in the array
	sem_post(&(sh_mem->mutex));
	// Exit CS (Insert rec id in the array)

	// Enter CS (Write)
	sem_wait(&(sh_mem->sem_writers_recs[rec_index]));
	// Enter CS (Search records)
	sem_wait(&(sh_mem->mutex));

	// There is one or more writers who write on this record
	for (int i = 0; i < rec_index; i++) {
		if (sh_mem->sem_writers_recs[i] == recid) {
			sem_post(&(sh_mem->mutex)); // Leave mutex, don't hold back the other processes
			sem_wait(&(sh_mem->sem_writers_recs[i])); // Writer is suspended until writers (who came before) finish
			sem_wait(&(sh_mem->mutex));
		}
	}
	// There is one or more readers who read on this record
	for (int i = 0; i < last_reader; i++) {
		if (sh_mem->sem_readers_recs[i] == recid) {
			sem_post(&(sh_mem->mutex)); // Leave mutex, don't hold back the other processes
			sem_wait(&(sh_mem->sem_readers_recs[i])); // Writer is suspended until readers (who came before) finish
			sem_wait(&(sh_mem->mutex));
		}
	}
	sem_post(&(sh_mem->mutex));
	// Exit CS (Search records)

	CHECK_CALL(write(rp, &value, sizeof(int)), -1);
	sem_post(&(sh_mem->sem_writers_recs[rec_index]));
	// Exit CS (Write)

	// Enter CS (Remove id of this rec from the array)
	sem_wait(&(sh_mem->mutex));
	sh_mem->writers_recs[rec_index] = 0;
	sem_post(&(sh_mem->mutex));
	// Exit CS (Remove id of this rec from the array)

	// Close file
	CHECK_CALL(close(rp), -1);

	// Enter CS (Increase records processed)
	sem_wait(&(sh_mem->sem_sum));
	sh_mem->total_recs_processed += num_recs; // Increase number of records that have been processed
	sem_wait(&(sh_mem->sem_sum));
	// Exit CS (Increase records processed)

	// Enter CS (Remove writers's pid from the array of writers' pids)
	sem_wait(&(sh_mem->sem_new_writer));
	sh_mem->writers_pid[pid_index] = 0;
	sem_post(&(sh_mem->sem_new_writer));
	// Exit CS (Remove writers's pid from the array of writers' pids)

	printf("Writer %d\n", pid);

	// Calculate time
	t2 = (double)times(&tb2);
	realtime = (double)((t2 - t1) / tics_per_sec);
	sh_mem->time_readers[rec_index] = realtime;

	// Detach shared memory segment
	CHECK_CALL(err = shmdt((void *) sh_mem), -1);

	exit(0);
}
