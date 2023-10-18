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
	(*list)->count = 0;
}

// Insert a voter in the list
void Insert_List(Listptr *list, Voter *voter) {
	Listptr prev, current, new_n;
	ListNode *temp_prev, *temp;
	current = *list;
	prev = NULL;

	// First insert
	if ((*list)->zipcode == -1) {
		(*list)->zipcode = voter->zipcode;
		(*list)->count = 1;
		CHECK_MALLOC_NULL((*list)->voters = malloc(sizeof(ListNode)));
		(*list)->voters->voter = voter;
		(*list)->voters->next_voter = NULL;
		return;
	}

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
			CHECK_MALLOC_NULL(temp_prev->next_voter = malloc(sizeof(ListNode)));
			temp_prev->next_voter->voter = voter;
			temp_prev->next_voter->next_voter = NULL;
			return;
		}
		current = current->next_node;
	}

	CHECK_MALLOC_NULL(current = malloc(sizeof(ListNode_2d)));
	current->zipcode = voter->zipcode;
	current->count = 1;
	current->next_node = (*list);
	CHECK_MALLOC_NULL(current->voters = malloc(sizeof(ListNode)));
	current->voters->voter = voter;
	current->voters->next_voter = NULL;
	(*list) = current;
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
	printf("0 voted in %d\n\n", zip);
}

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