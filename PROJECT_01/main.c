#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "voter.h"
#include "hash_table.h"
#include "2d_list.h"

#define N 5
#define SIZE 100

int main (int argc, char *argv[]) { /* argv[0]='mvote' */

	// HTptr *table_entries;
	Listptr list_voted;
	FILE *fp, *fileofkeys;
	int bucketentries, i, pin, zip, key, argv_file, argv_buck, count_voted;
	float percent;
	char *lname, *fname, *input, *command;

	// Check argument values
	if (argc < 5) {
		printf("Not enough arguments!\n");
		exit(1);
	}

	if ((strcmp(argv[1], "-f") != 0 && strcmp(argv[1], "-b") != 0) || (strcmp(argv[3], "-f") != 0 && strcmp(argv[3], "-b") != 0)) {
		printf("Error with flags!\n");
		exit(1);
	}
	else if ((strcmp(argv[1], "-b") == 0 && strcmp(argv[3], "-b") == 0) || (strcmp(argv[1], "-f") == 0 && strcmp(argv[3], "-f") == 0)) {
		printf("Error with flags!\n");
		exit(1);
	}

	if (strcmp(argv[1], "-f") == 0) {
		argv_file = 2;
		argv_buck = 4;
		
	}
	else {
		argv_file = 4;
		argv_buck = 2;
	}

	// Open file
	fp = fopen(argv[argv_file], "r");
	if (!fp) {
    	printf("Can't open file!\n");
    	exit(1);
    }

	// Bucket entries
	for (i = 0; i < strlen(argv[argv_buck]); i++) {
		if (isdigit(argv[argv_buck][i]) == 0) {
			printf("Bucket entries must be a positive integer!\n");
			exit(1);
		}
	}
	bucketentries = atoi(argv[argv_buck]);

	// Create buffers for scanf
	command = malloc(N * sizeof(char));
	input = malloc(SIZE * sizeof(char));
	if (command == NULL || input == NULL){
		printf("Cannot allocate memory!\n");
		exit(1);
	}

	// Create hash table


	// Create 2d list
	Create_List(&list_voted);

	// Commands to mvote from command line
	while (1) {
		scanf("%s", command);
		printf("\n");

		// Case 1
		if (strcmp(command, "l") == 0) {
			scanf("%d", &pin);
//			printf("pin: %d", pin);
//			printf("\nCase1 done\n");
		}

		// Case 2
		else if (strcmp(command, "i") == 0) {
			scanf("%d", &pin);
			scanf("%s", input); // lname
//			printf("pin: %d, lname: %s", pin, input);
			scanf("%s", input); // fname
			scanf("%d", &zip);
//			printf("fname: %s, zip: %d", input, zip);
//			printf("\nCase2 done\n");
		}

		// Case 3
		else if (strcmp(command, "m") == 0) {
			scanf("%d", &pin);
			printf("%d Market Voted", pin);
		}

		// Case 4
		else if (strcmp(command, "bv") == 0) {
			scanf("%s", input); // file name
			fileofkeys = fopen(input, "r");
			if (!fileofkeys) {
    			printf("Can't open file!\n");
    			break;
    		}
			while (fscanf(fileofkeys, "%d", &key) == 1) {
//				printf("%d ", key);
			}
			fclose(fileofkeys);
//			printf("\nCase4 done\n");
		}

		// Case 5
		else if (strcmp(command, "v") == 0) {
			// Return the size of the 2d list
			printf("Voted So Far: %d\n", Size_List(list_voted));
		}

		// Case 6
		else if (strcmp(command, "perc") == 0) {
			// Find the percent
			//total = 
			count_voted =  Size_List(list_voted);
			//percent = float(count_voted) / float(total);
			printf("%2.4f\n", percent);
		}

		// Case 7
		else if (strcmp(command, "z") == 0) {
			scanf("%d", &zip);
//			printf("pin: %d", zip);
		}

		// Case 8
		else if (strcmp(command, "o") == 0) {

		}

		// Case 9
		else if (strcmp(command, "exit") == 0) {
			printf("Exiting the program! Goodbye!\n");
			break;
		}
	}

	// Free memory
	fclose(fp);
	free(command);
	free(input);
	Delete_List(&list_voted);

	return 0;
}