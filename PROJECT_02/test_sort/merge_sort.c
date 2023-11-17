#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header.h"

void merge(Record **voters, int begin, int middle, int end) {

	int i = 0, j = 0, h = begin;
	int first_size = middle - begin + 1; // Size of first array
	int second_size = end - middle; // Size of second array
	Record rec;

	// Create temp arrays
	Record *first_array, *second_array;
	CHECK_MALLOC_NULL(first_array = malloc(first_size * sizeof(Record)));
	CHECK_MALLOC_NULL(second_array = malloc(second_size * sizeof(Record)));

	// Copy data to temp arrays
	for (i = 0; i < first_size; i++) {
		first_array[i] = (*voters)[begin + i];
	}
	for (i = 0; i < second_size; i++) {
		second_array[i] = (*voters)[middle + i + 1];
	}
	i = 0;

	// Sort the records
	while (i < first_size && j < second_array) {
		if (strcmp(first_array[i].LastName, second_array[j].LastName) < 0) {
			(*voters)[h] = first_array[i];
			i++;
		}
		// Same last name
		else if (strcmp(first_array[i].LastName, second_array[j].LastName) == 0) {
			if (strcmp(first_array[i].FirstName, second_array[j].FirstName) < 0) {
				(*voters)[h] = first_array[i];
				i++;
			}
			// Same last name and first name
			else if (strcmp(first_array[i].FirstName, second_array[j].FirstName) == 0) {
				if (first_array[i].custid < second_array[j].custid) {
					(*voters)[h] = first_array[i];
					i++;
				}
			}
		}
		else {
			(*voters)[h] = second_array[j];
			j++;
		}
		h++;
	}

	// Copy remaining voters from first array (if there are any)
	while (i < first_size) {
		(*voters)[h] = first_array[i];
		i++;
		h++;
	}
	// Copy remaining voters from second array (if there are any)
	while (j < second_size) {
		(*voters)[h] = second_array[j];
		j++;
		h++;
	}

	free(first_array);
	free(second_array);
}

void merge_sort(Record **voters, int first, int last) {
	if (first < last) {
		int middle = first + (last - first) / 2;
		merge_sort(voters, first, middle);
		merge_sort(voters, middle + 1, last);

		merge(voters, first, middle, last);
	}
}
