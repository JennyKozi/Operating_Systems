#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "voter.h"
#include "hash_table.h"
#include "2d_list.h"

#define SIZE 40 // size for buffers

// Check if the memory is allocated and malloc didn't fail
#define CHECK_MALLOC_NULL(p)  \
if ((p) == NULL) {  \
	printf("Cannot allocate memory!\n"); \
	exit(1);  \
};

// Global variable to count bytes allocted
long int num_bytes = 0;

// Check input from user
int Check_Int(char *);
int Check_String(char *);

int main (int argc, char *argv[]) { /* argv[0]='mvote' */

	Listptr list_voted;
	HTptr table_voters;
	FILE *fp, *fileofkeys;
	Voter *voter;
	int bucketentries, i, pin, zip, argv_file, argv_buck, count_voted, total;
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
	num_bytes = num_bytes + (4 * SIZE * sizeof(char));

	// Create hash table
	Create_HT(&table_voters, bucketentries);

	// Create 2d list
	Create_List(&list_voted);

	// Read the voters from the file
	while (fscanf(fp, "%d", &pin) == 1) { // Scan pin

		// Check for duplicates (not allowed)
		if (Search_HT(table_voters, pin) != NULL) {
			printf("Duplicates are not allowed!\n");
			free(command);
			free(input);
			free(lname);
			free(fname);
			Delete_HT(&table_voters);
			Delete_List(&list_voted);
			fclose(fp);
			exit(1);
		}

		fscanf(fp, "%s", fname); // Scan first name
		fscanf(fp, "%s", lname); // Scan last name
		fscanf(fp, "%d", &zip); // Scan zipcode
		
		// Create voter
		CHECK_MALLOC_NULL(voter = malloc(sizeof(Voter)));
		CHECK_MALLOC_NULL(voter->first_name = malloc(strlen(fname) + 1)); // Allocate memory for first name
		CHECK_MALLOC_NULL(voter->last_name = malloc(strlen(lname) + 1)); // Allocate memory for last name
		num_bytes = num_bytes + sizeof(Voter) + strlen(fname) + strlen(lname) + 2; // Increase bytes allocated with malloc

		strcpy(voter->last_name, lname); // Write first name
		strcpy(voter->first_name, fname); // Write last name
		voter->has_voted = 'N'; // They haven't voted
		voter->PIN = pin; // We have scanned their unique pin
		voter->zipcode = zip; // Write zipcode

		Insert_HT(table_voters, voter); // Insert voter in hash table
	}
	fclose(fp);

	printf("Insert command:\n\n");

	// Commands to mvote from command line
	while (1) {
		scanf("%s", command);

		// CASE 1
		if (strcmp(command, "l") == 0) {

			// Check if pin is int
			scanf("%s", input);
			if (Check_Int(input) == 0) {
				printf("Malformed Pin\n\n");
				continue;
			}
			pin = atoi(input);

			// Check if voter is in hash table
			voter = Search_HT(table_voters, pin);
			if (voter != NULL) { // Voter exists
				printf("%d %s %s %d %c\n\n", voter->PIN, voter->last_name, voter->first_name, voter->zipcode, voter->has_voted);
			}
			else { // Voter doesn't exist
				printf("Participant %d not in cohort\n\n", pin);
			}
		}

		// CASE 2
		else if (strcmp(command, "i") == 0) {

			// Check if pin is int
			scanf("%s", input);
			if (Check_Int(input) == 0) {
				printf("Malformed Input\n\n");
				continue;
			}
			pin = atoi(input);
			
			// Check if voter exists
			voter = Search_HT(table_voters, pin);
			if (voter != NULL) { // Voter doesn't exist
				printf("%d already exists\n\n", pin);
				continue;
			}
			// Last name must not contain numbers
			scanf("%s", lname);
			if (Check_String(lname) == 0) {
				printf("Malformed Input\n\n");
				continue;
			}
			// First name must not contain numbers
			scanf("%s", fname);
			if (Check_String(fname) == 0) {
				printf("Malformed Input\n\n");
				continue;
			}
			// Zip must be an int
			scanf("%s", input);
			if (Check_Int(input) == 0) {
				printf("Malformed Input\n\n");
				continue;
			}
			zip = atoi(input);
			
			// Create voter
			CHECK_MALLOC_NULL(voter = malloc(sizeof(Voter)));
			CHECK_MALLOC_NULL(voter->last_name = malloc(strlen(lname) + 1)); // Allocate memory for first name
			CHECK_MALLOC_NULL(voter->first_name = malloc(strlen(fname) + 1)); // Allocate memory for last name
			num_bytes = num_bytes + sizeof(Voter) + strlen(fname) + strlen(lname) + 2; // Increase bytes allocated with malloc

			voter->has_voted = 'N'; // They haven't voted
			voter->PIN = pin; // We have scanned their unique pin
			strcpy(voter->last_name, lname); // Copy first name
			strcpy(voter->first_name, fname); // Copy last name
			voter->zipcode = zip;

			Insert_HT(table_voters, voter); // Insert voter in hash table
			printf("Inserted %d %s %s %d N\n\n", pin, lname, fname, zip);
		}

		// CASE 3
		else if (strcmp(command, "m") == 0) {

			// Check if pin is int
			scanf("%s", input);
			if (Check_Int(input) == 0) {
				printf("Malformed Input\n\n");
				continue;
			}
			pin = atoi(input);
			
			// Check if voter exists
			voter = Search_HT(table_voters, pin);
			// Not found
			if (voter == NULL) {
				printf("%d does not exist\n\n", pin);
				continue;
			}
			// Found but has already voted
			if (voter->has_voted == 'Y') {
				printf("%d Marked Voted\n\n", pin);
			}
			// Found but hasn't voted
			else {
				voter->has_voted = 'Y';
				printf("%d Marked Voted\n\n", pin);
				Insert_List(&list_voted, voter);
			}
		}

		// CASE 4
		else if (strcmp(command, "bv") == 0) {

			scanf("%s", input); // file name
			fileofkeys = fopen(input, "r");
			if (!fileofkeys) {
    			printf("%s could not be opened\n\n", input);
    			continue;
    		}
			// Read keys from file
			while (fscanf(fileofkeys, "%s", input) == 1) {
				// Pin must be int
				if (Check_Int(input) == 0) {
					printf("Malformed Input\n");
					continue;
				}
				pin = atoi(input);

				// Check if voter exists
				voter = Search_HT(table_voters, pin);
				// Not found
				if (voter == NULL) {
					printf("%d does not exist\n", pin);
					continue;
				}
				// Found but has already voted
				if (voter->has_voted == 'Y') {
					printf("%d Marked Voted\n", pin);
				}
				// Found but hasn't voted
				else {
					voter->has_voted = 'Y';
					printf("%d Marked Voted\n", pin);
					Insert_List(&list_voted, voter);
				}
			}
			printf("\n");
			fclose(fileofkeys);
		}

		// CASE 5
		else if (strcmp(command, "v") == 0) {
			// Return the size of the 2d list
			printf("Voted So Far: %d\n\n", Size_List(list_voted));
		}

		// CASE 6: find the percent
		else if (strcmp(command, "perc") == 0) {
			total = table_voters->num_keys;
			count_voted = Size_List(list_voted);
			percent = ((float)count_voted) / total;
			printf("%.5f\n\n", percent);
		}

		// CASE 7
		else if (strcmp(command, "z") == 0) {
			scanf("%d", &zip);
			Print_Zip_List(list_voted, zip);
		}

		// CASE 8
		else if (strcmp(command, "o") == 0) {
			Print_Descending_List(list_voted);
		}

		// CASE 9
		else if (strcmp(command, "exit") == 0) {
			printf("%ld of Bytes Released\n", num_bytes);
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

// Check that it is an int
int Check_Int(char *string) {

	int i;
	// Check for chars
	for (i = 0; i < strlen(string); i++) {
		if (isalpha(string[i])) {
			return 0;
		}
	}
	// Check if num is float
	float float_num = atof(string);
	int num = float_num;
	float diff = float_num - (float)num;

	if (diff != 0.0) {
		return 0;
	}
	return 1;
}

// Check that string does not contain numbers
int Check_String(char *string) {
	int i;
	for (i = 0; i < strlen(string); i++) {
		if (isdigit(string[i])) {
			return 0;
		}
	}
	return 1;
}