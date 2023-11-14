#define SIZE_NAME 20
#define SIZE_POST 6

typedef struct {
	int custid;
	char LastName[SIZE_NAME];
	char FirstName[SIZE_NAME];
	char postcode[SIZE_POST];
} Record;

// Check if the memory is allocated and malloc didn't fail
#define CHECK_MALLOC_NULL(p)  \
if ((p) == NULL) {  \
	printf("Cannot allocate memory!\n"); \
	exit(1);  \
};

// Check if the file was opened
#define CHECK_FILE(f)  \
if ((f) == -1) {  \
	perror("Cannot open file!\n");  \
	exit(1);  \
};
