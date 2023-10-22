#include <stdio.h>
#include <stdlib.h>
#include "voter.h"
#include "2d_list.h"

// Check if the memory is allocated and malloc didn't fail
#define CHECK_MALLOC_NULL(p)  \
if ((p) == NULL) {  \
	printf("Cannot allocate memory!\n"); \
	exit(1);  \
};

extern long int num_bytes;

// Create 2d_list
void Create_List(Listptr *list) {

	CHECK_MALLOC_NULL((*list) = malloc(sizeof(ListNode_2d)));
	num_bytes = num_bytes + sizeof(ListNode_2d);
	(*list)->voters = NULL;
	(*list)->next_node = NULL;
	(*list)->zipcode = -1;
	(*list)->count = 0;
}

// Insert a voter in the list (sorted insert)
void Insert_List(Listptr *list, Voter *voter) {
	Listptr prev = NULL, current, curr_n, prev_n = NULL;
	ListNode *temp_prev, *temp;
	current = *list;

	// First insert of a voter (empty list)
	if ((*list) != NULL) {
		if ((*list)->zipcode == -1) {
			(*list)->zipcode = voter->zipcode;
			(*list)->count = 1;
			CHECK_MALLOC_NULL((*list)->voters = malloc(sizeof(ListNode)));
			num_bytes = num_bytes + sizeof(ListNode);
			(*list)->voters->voter = voter;
			(*list)->voters->next_voter = NULL;
			return;
		}
	}

	// Find if there is a node with the new voter's zip
	while (current != NULL) {
		// Found the node with the correct zip
		if (current->zipcode == voter->zipcode) {
			temp = current->voters;
			temp_prev = current->voters;
			current->count++;
			while (temp != NULL) {
				temp_prev = temp;
				temp = temp->next_voter;
			}
			// Create node for new voter
			CHECK_MALLOC_NULL(temp_prev->next_voter = malloc(sizeof(ListNode)));
			num_bytes = num_bytes + sizeof(ListNode); // Increase bytes allocated
			temp_prev->next_voter->voter = voter;
			temp_prev->next_voter->next_voter = NULL;

			// Ckeck if the node with the new voter has to change place
			curr_n = (*list);
			while (curr_n != NULL && curr_n != current) {
				if ((current->count > curr_n->count) && prev != NULL && prev_n != NULL) {
					prev->next_node = current->next_node;
					prev_n->next_node = current;
					current->next_node = curr_n;
					return;
				}
				// Insert in the beggining of the list
				if ((current->count > curr_n->count) && prev_n == NULL) { // curr_n is the first node of the list
					prev->next_node = current->next_node;
					current->next_node = (*list);
					(*list) = current;
					return;
				}
				prev_n = curr_n;
				curr_n = curr_n->next_node;
			}
			return;
		}
		prev = current;
		current = current->next_node;
	}

	// Create a new node in the end of the list (no one with this zip has voted yet)
	if (prev != NULL) {
		// Create 2d node
		CHECK_MALLOC_NULL(prev->next_node = malloc(sizeof(ListNode_2d)));
		num_bytes = num_bytes + sizeof(ListNode_2d); // Increase bytes allocated
		prev->next_node->zipcode = voter->zipcode;
		prev->next_node->count = 1;
		prev->next_node->next_node = NULL;
		// Create node and insert voter
		CHECK_MALLOC_NULL(prev->next_node->voters = malloc(sizeof(ListNode)));
		num_bytes = num_bytes + sizeof(ListNode); // Increase bytes allocated
		prev->next_node->voters->voter = voter;
		prev->next_node->voters->next_voter = NULL;
	}
}

// Count how many people have voted
int Size_List(Listptr list) {

	int total = 0;
	Listptr temp = list;
	while (temp != NULL) {
		total += temp->count;
		temp = temp->next_node;
	}
	return total;
}

// Print info of zip
void Print_Zip_List(Listptr list, int zip) {

	Listptr current = list;
	ListNode *temp;
	while (current != NULL) {
		if (current->zipcode == zip) {
			printf("%d voted in %d\n", current->count, zip);
			temp = current->voters;
			while (temp != NULL) {
				printf("%d\n", temp->voter->PIN);
				temp = temp->next_voter;
			}
			printf("\n");
			return;
		}
		current = current->next_node;
	}
	printf("0 voted in %d\n\n", zip); // No one with this zip was found
}

// Print list in a descending order based on who many have voted with the same zip
void Print_Descending_List(Listptr list) {

	Listptr current = list;
	while (current != NULL) {
		printf("%d %d\n", current->zipcode, current->count);
		current = current->next_node;
	}
	printf("\n");
}

// Delete 2d list, free memory
void Delete_List(Listptr *list) {

	Listptr temp_node;
	ListNode *temp_voter;
	while ((*list) != NULL) {
		temp_node = (*list)->next_node;
		while ((*list)->voters != NULL) {
			temp_voter = (*list)->voters->next_voter;
			free((*list)->voters);
			(*list)->voters = temp_voter;
		}
		free(*list);
		*list = temp_node;
	}
}