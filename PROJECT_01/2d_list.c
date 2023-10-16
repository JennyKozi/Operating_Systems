#include <stdio.h>
#include <stdlib.h>
#include "voter.h"
#include "2d_list.h"

#define CHECK_MALLOC_NULL(p)  \
if ((p) == NULL) {  \
	printf("Cannot allocate memory!\n"); \
	exit(1);  \
};

extern int num_bytes;

// Create 2d_list
void Create_List(Listptr *list) {

	CHECK_MALLOC_NULL((*list) = malloc(sizeof(ListNode_2d)));
	(*list)->voters = NULL;
	(*list)->next_node = NULL;
	(*list)->zipcode = -1;
}

// Insert a voter in the list
void Insert_List(Listptr *list, Voter *voter) {
	Listptr prev, current, new_n;
	ListNode *curr;
	current = *list;
	prev = NULL;

	// First insert
	if ((*list)->zipcode == -1) {
		(*list)->zipcode = voter->zipcode;
		CHECK_MALLOC_NULL((*list)->voters = malloc(sizeof(ListNode)));
		(*list)->voters->voter = voter;
		(*list)->voters->next_voter = NULL;
		return;
	}

	while (current != NULL) {
		// Found the node with the correct zip
		if (current->zipcode == voter->zipcode) {
			curr = current->voters;
			while (curr != NULL) {
				curr = curr->next_voter;
			}
			CHECK_MALLOC_NULL(curr = malloc(sizeof(ListNode)));
			curr->voter = voter;
			curr->next_voter = NULL;
			return;
		}
		// Zip of current node less than zip of new voter => Go to the next node
		else if (current->zipcode < voter->zipcode) {
			prev = current;
			current = current->next_node;
		}
		else { // current->zipcode > voter->zipcode
			CHECK_MALLOC_NULL(new_n = malloc(sizeof(ListNode_2d)));
			new_n->next_node = current;
			new_n->zipcode = voter->zipcode;
			CHECK_MALLOC_NULL(new_n->voters = malloc(sizeof(ListNode)));
			new_n->voters->voter = voter;
			new_n->voters->next_voter = NULL;
			if (prev != NULL) {
				prev->next_node = new_n;
			}
			else {
				(*list) = new_n;
			}
			return;
		}
	}

	// New zip greater than current greatest zip on the list => Create new node at the end
	CHECK_MALLOC_NULL(current = malloc(sizeof(ListNode_2d)));
	current->next_node = NULL;
	current->zipcode = voter->zipcode;
	CHECK_MALLOC_NULL(current->voters = malloc(sizeof(ListNode)));
	current->voters->voter = voter;
	current->voters->next_voter = NULL;
}

// Count how many people have voted
int Size_List(Listptr list) {
	int count = 0;
	ListNode *temp;
	while (list != NULL) {
		temp = list->voters;
		while (temp != NULL) {
			if (temp->voter != NULL) {
				count++;
			}
			temp = temp->next_voter;
		}
		list = list->next_node;
	}
	return count;
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