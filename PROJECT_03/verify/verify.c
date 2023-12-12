#include <stdio.h>
#include <unistd.h>

#define SIZE 20

typedef struct {
	int customer_id;
	char last_name[SIZE];
	char first_name[SIZE];
	int	balance;
} Record;

int main (int argc, char** argv) {

	FILE *fpb;
	Record rec;
	long file_size;
	int numof_records, i;

	if (argc != 2) {
		printf("Wrong number of arguments!\n");
		return 1;
   	}
	fpb = fopen (argv[1], "rb");
	if (fpb == NULL) {
		printf("Cannot open binary file!\n");
		return 1;
	}

	// check number of records
	fseek(fpb, 0, SEEK_END);
	file_size = ftell (fpb);
	rewind(fpb);
	numof_records = (int)file_size / sizeof(rec);

	printf("\nRecords found in file: %d\n\n", numof_records);

	for (i = 0; i < numof_records; i++) {
		fread(&rec, sizeof(rec), 1, fpb);
		printf("%d %s %s %d\n", rec.customer_id, rec.last_name, rec.first_name, rec.balance);
	}
	print("\n");

	fclose (fpb);
	return 0;
}
