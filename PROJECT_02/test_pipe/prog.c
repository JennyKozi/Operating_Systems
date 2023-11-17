#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "header.h"

int main(int argc, char *argv[]) {

	int rp = atoi(argv[1]);
	int pipe = atoi(argv[2]);
	Record rec;
//	Record array[5];

	for (int i = 0; i < 10; i++) {
		read(rp, &rec, sizeof(rec));
//		array[i] = rec;
		write(pipe, &rec.custid, sizeof(int));
		write(pipe, rec.FirstName, sizeof(rec.FirstName));
		write(pipe, rec.LastName, sizeof(rec.LastName));
		write(pipe, rec.postcode, sizeof(rec.postcode));
//		printf("%d %s %s %s\n", rec.custid, rec.FirstName, rec.LastName, rec.postcode);
	}
//	printf("\n");
	close(pipe);

	return 0;
}
