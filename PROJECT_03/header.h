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
# include <semaphore.h>
# include <sys/ipc.h>
# include <sys/shm.h>
#include <sys/wait.h>

#define SIZE 20

typedef struct {
	int customer_id;
	char last_name[SIZE];
	char first_name[SIZE];
	int	balance;
} Record;

typedef struct  {
	int num_readers;
	int num_writers;
	int num_recs_processed;
	sem_t new_reader, new_writer;
} shared_mem_seg;

// Check if the memory is allocated and malloc didn't fail
#define CHECK_MALLOC_NULL(p)  \
if ((p) == NULL) {  \
	perror("Cannot allocate memory!\n"); \
	exit(1);  \
};

// Check if the file was opened
#define CHECK_FILE(f)  \
if ((f) == -1) {  \
	perror("Cannot open file!\n");  \
	exit(1);  \
};
