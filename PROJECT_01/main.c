#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "voter.h"
#include "hash_table.h"
#include "2d_list.h"

#define SIZE 40

#define CHECK_MALLOC_NULL(p)  \
if ((p) == NULL) {  \
	printf("Cannot allocate memory!\n"); \
	exit(1);  \
};

// Global variable to count bytes allocted
int num_bytes = 0;

int Check_Int(float);

int main (int argc, char *argv[]) { /* argv[0]='mvote' */

	Listptr list_voted;
	HTptr table_voters;
	FILE *fp, *fileofkeys;
	Voter *voter;
	int bucketentries, i, pin, zip, key, argv_file, argv_buck, count_voted, flag = 0;
	float percent, float_num;
	char *input, *lname, *fname, *command;

	// Check for errors
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

	// Take values from flags
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
	CHECK_MALLOC_NULL(command = malloc(SIZE * sizeof(char)));
	CHECK_MALLOC_NULL(input = malloc(SIZE * sizeof(char)));
	CHECK_MALLOC_NULL(lname = malloc(SIZE * sizeof(char)));
	CHECK_MALLOC_NULL(fname = malloc(SIZE * sizeof(char)));

	// Create hash table
	Create_HT(&table_voters, bucketentries);

	// Create 2d list
	Create_List(&list_voted);

	// Read the voters from the file
	while (fscanf(fp, "%d", &pin) == 1) {

		fscanf(fp, "%s", fname); // Scan first name
		fscanf(fp, "%s", lname); // Scan last name
		fscanf(fp, "%d", &zip); // Scan zipcode
		
		// Create voter
		CHECK_MALLOC_NULL(voter = malloc(sizeof(Voter)));
		CHECK_MALLOC_NULL(voter->first_name = malloc(strlen(fname) + 1));
		CHECK_MALLOC_NULL(voter->last_name = malloc(strlen(lname) + 1));

		strcpy(voter->last_name, lname);
		strcpy(voter->first_name, fname);
		voter->has_voted = 'N'; // They haven't voted
		voter->PIN = pin; // We have scanned their unique pin
		voter->zipcode = zip;

		Insert_HT(table_voters, voter); // Insert voter in hash table
	}
	fclose(fp);

	printf("Insert command:\n");

	// Commands to mvote from command line
	while (1) {
		scanf("%s", command);

		// CASE 1
		if (strcmp(command, "l") == 0) {

			// Check if pin is int
			scanf("%f", &float_num);
			if (Check_Int(float_num) == 0) {
				printf("Malformed Pin\n\n");
				continue;
			}
			pin = float_num;
			
			// Check if voter os in hash table
			voter = Search_HT(table_voters, pin);
			if (voter != NULL) {
				printf("%d %s %s %d %c\n\n", voter->PIN, voter->last_name, voter->first_name, voter->zipcode, voter->has_voted);
			}
			else {
				printf("Participant %d not in cohort\n\n", pin);
			}
		}

		// CASE 2
		else if (strcmp(command, "i") == 0) {

			// Check if pin is int
			scanf("%f", &float_num);
			if (Check_Int(float_num) == 0) {
				printf("Malformed Input\n\n");
				continue;
			}
			pin = float_num;
			
			// Check if voter exists
			voter = Search_HT(table_voters, pin);
			if (voter != NULL) {
				printf("%d already exists\n\n", pin);
				continue;
			}
			// Last name must not contain numbers
			scanf("%s", lname);
			for (i = 0; i < strlen(lname); i++) {
				if (isdigit(lname[i])) {
					printf("Malformed Input\n\n");
					flag = 1;
					break;
				}
			}
			if (flag == 1) {
				flag = 0;
				continue;
			}
			// First name must not contain numbers
			scanf("%s", fname);
			for (i = 0; i < strlen(fname); i++) {
				if (isdigit(fname[i])) {
					printf("Malformed Input\n\n");
					flag = 1;
					continue;
				}
			}
			if (flag == 1) {
				flag = 0;
				continue;
			}
			// Zip must be an int
			scanf("%f", &float_num);
			if (Check_Int(float_num) == 0) {
				printf("Malformed Input\n\n");
				continue;
			}
			zip = float_num;
			
			// Create voter
			CHECK_MALLOC_NULL(voter = malloc(sizeof(Voter)));
			CHECK_MALLOC_NULL(voter->last_name = malloc(strlen(lname) + 1));
			CHECK_MALLOC_NULL(voter->first_name = malloc(strlen(fname) + 1));

			voter->has_voted = 'N'; // They haven't voted
			voter->PIN = pin; // We have scanned their unique pin
			strcpy(voter->last_name, lname);
			strcpy(voter->first_name, fname);
			voter->zipcode = zip;

			Insert_HT(table_voters, voter); // Insert voter in hash table
			printf("Inserted %d %s %s %d N\n\n", pin, lname, fname, zip);
		}

		// CASE 3
		else if (strcmp(command, "m") == 0) {
			// Check if pin is int
			scanf("%f", &float_num);
			if (Check_Int(float_num) == 0) {
				printf("Malformed Input\n\n");
				continue;
			}
			pin = float_num;
			
			// Check if voter exists
			voter = Search_HT(table_voters, pin);
			if (voter == NULL) {
				printf("%d does not exist\n\n", pin);
				continue;
			}
			voter->has_voted = 'Y';
			printf("%d Marked Voted", pin);
			
		}

		// CASE 4
		else if (strcmp(command, "bv") == 0) {
			scanf("%s", input); // file name
			fileofkeys = fopen(input, "r");
			if (!fileofkeys) {
    			printf("%s could not be opened\n", input);
    			continue;
    		}
			while (fscanf(fileofkeys, "%d", &key) == 1) {

			}
			fclose(fileofkeys);
		}

		// CASE 5
		else if (strcmp(command, "v") == 0) {
			// Return the size of the 2d list
			printf("Voted So Far: %d\n", Size_List(list_voted));
		}

		// CASE 6: find the percent
		else if (strcmp(command, "perc") == 0) {
			//total = 
			count_voted =  Size_List(list_voted);
			//percent = ((float)count_voted) / total;
			printf("%.4f\n", percent);
		}

		// CASE 7
		else if (strcmp(command, "z") == 0) {
			scanf("%d", &zip);

		}

		// CASE 8
		else if (strcmp(command, "o") == 0) {

		}

		// CASE 9
		else if (strcmp(command, "exit") == 0) {
			printf(" of Bytes Released\n");
			break;
		}
	}

	// Free memory
	free(command);
	free(input);
	free(lname);
	free(fname);
	Delete_HT(&table_voters);
	Delete_List(&list_voted);

	return 0;
}

// Functions
int Check_Int(float float_num) {
	int num = float_num;
	float diff = float_num - (float)num;

	if (diff != 0.0) {
		return 0;
	}
	return 1;
}