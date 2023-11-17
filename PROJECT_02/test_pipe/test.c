#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "header.h"

int main() {

	int rp, size, total_records, stat, p[2], nbytes;
	Record rec;
	struct stat buffer;

	// Open binary data file
	if ((rp = open("voters50.bin", O_RDONLY)) == -1) {
		perror("Cannot open file!\n");
		exit(1);
	}

	// Get number of records
	stat = fstat(rp, &buffer);
	if (stat != 0) {
		perror("Error reading file!\n");
		close(rp);
		exit(1);
	}
	size = buffer.st_size;
	total_records = (int)size / sizeof(rec);
	lseek(rp, 0, SEEK_SET);

	read(rp, &rec, sizeof(rec));
//	lseek(rp, sizeof(rec), SEEK_SET);

	printf("%d %s %s %s\n\n", rec.custid, rec.FirstName, rec.LastName, rec.postcode);

	// Create a pipe
	if (pipe(p) < 0) {
		perror("Error with pipe!\n");
		exit(1);
	}
	char str1[10];
	char str2[16];
	snprintf(str1, sizeof(str1), "%d", rp);
	snprintf(str2, sizeof(str2), "%d", p[1]);

	pid_t child = fork();
	if (child == -1) {
		perror("Failed to fork!\n");
		exit(1);
	}
	else if (child == 0) {
		close(p[0]);
		execl("./prog", "./prog", str1, str2, (char *)NULL);
		perror("exec failure!\n");
		exit(1);
	}
	else {
		close(p[1]);
//		pid_t cpid = wait(NULL);

		while (nbytes = read(p[0], &rec.custid, sizeof(int)) > 0) {
			read(p[0], rec.FirstName, sizeof(rec.FirstName));
			read(p[0], rec.LastName, sizeof(rec.LastName));
			read(p[0], rec.postcode, sizeof(rec.postcode));
			printf("%d %s %s %s\n", rec.custid, rec.LastName, rec.FirstName, rec.postcode);
		}
		pid_t cpid = waitpid(child, NULL, WNOHANG);
		printf("\nParent pid = %d\n", getpid());
		close(p[0]);
//    	printf("Child pid = %d\n", cpid);
	}

	return 0;
}
