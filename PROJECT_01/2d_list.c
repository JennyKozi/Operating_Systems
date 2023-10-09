#include <stdio.h>
#include <stdlib.h>
#include "voter.h"
#include "2d_list.h"

// Functions for 2d list

void Create_List(Listptr *list) {

	(*list) = malloc(sizeof(ListNode_2d));
	if ((*list) == NULL) {
		printf("Cannot allocate memory!\n");
		exit(1);
	}
	(*list)->next_node = NULL;
	(*list)->zipcode = 0;

	// Voters list
	(*list)->voters = malloc(sizeof(ListNode));
	if ((*list)->voters == NULL) {
		printf("Cannot allocate memory!\n");
		exit(1);
	}
	(*list)->voters->next_voter = NULL;
	(*list)->voters->voter = NULL;
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

// Free memory
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