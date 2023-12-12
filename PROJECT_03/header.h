#define SIZE 20

typedef struct {
	int customer_id;
	char last_name[SIZE];
	char first_name[SIZE];
	int	balance;
} Record;

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
