#include <stdio.h>
#include <stdlib.h>
#include "voter.h"
#include "hash_table.h"

#define M 5
#define L 0.75

static void Initialize_Bucket(Bucket **, int);
static void Insert_Bucket(Bucket **, Voter *, int);
static void Bucket_Split(HTptr ht, Bucket **, Bucket **);

// Create hash table
void Create_HT(HTptr *ht, int num_entries) {
	int i;

	(*ht) = malloc(sizeof(HT));
	if ((*ht) == NULL) {
		printf("Cannot allocate memory!\n");
		exit(1);
	}
	(*ht)->m = M;
	(*ht)->p = 0;
	(*ht)->num_buckets = M;
	(*ht)->num_keys = 0;
	(*ht)->bucketentries = num_entries;
	(*ht)->buckets = malloc(M * sizeof(Bucket *));
	if ((*ht)->buckets == NULL) {
		printf("Cannot allocate memory!\n");
		exit(1);
	}
	for (i = 0; i < M; i++) {
		(*ht)->buckets[i] = NULL;
	}
}

// Initialize an empty bucket
static void Initialize_Bucket(Bucket **bucket, int size) {
	int i;

	(*bucket)->next_bucket = NULL;
	(*bucket)->voters = malloc(size * sizeof(Voter *));
	if ((*bucket)->voters == NULL) {
		printf("Cannot allocate memory!\n");
		exit(1);
	}
	(*bucket)->count = 0;
	for (i = 0; i < size; i++) {
		(*bucket)->voters[i] = NULL;
	}
}

// Insert a voter into a bucket
static void Insert_Bucket(Bucket **bucket, Voter *voter, int size) {
	int i = 0;

	// Create a bucket
	if ((*bucket) == NULL) {
		(*bucket) = malloc(sizeof(Bucket));
		if ((*bucket) == NULL) {
			printf("Cannot allocate memory!\n");
			exit(1);
		}
		Initialize_Bucket(bucket, size);
	}
	
	if ((*bucket)->count == size) {
		Insert_Bucket(&((*bucket)->next_bucket), voter, size); // Overflow bucket
	}
	else {
		(*bucket)->voters[(*bucket)->count] = voter;
		(*bucket)->count++;
	}
}

// Insert a voter in the hash table
void Insert_HT(HTptr ht, Voter *voter) {
	int i = voter->PIN % ht->m;

	// The modulo points to a bucket that has splitted
	if (i < ht->p) {
		i = voter->PIN % (2 * (ht->m)); // Recalculate the index of the bucket
	}
	Insert_Bucket(&(ht->buckets[i]), voter, ht->bucketentries);
	ht->num_keys++;

	// Calculate threshold
	float l = ((float)ht->num_keys) / (ht->num_buckets * ht->bucketentries);

	// If l is greater than the threshold
	if (l > L) {
		ht->num_buckets++;
		ht->buckets = realloc(ht->buckets, ht->num_buckets * sizeof(Bucket *));
		ht->buckets[ht->num_buckets - 1] = NULL;
		Bucket_Split(ht, &(ht->buckets[ht->p]), &(ht->buckets[ht->num_buckets - 1]));
		ht->p++; // p points to the next bucket that will split
		// Complete round
		if (ht->p == 2 * ht->m) {
			ht->m = 2 * ht->m;
			ht->p = 0;
		}
	}
}

static void Bucket_Split(HTptr ht, Bucket **b1, Bucket **b2) {
	int i;
	Bucket *curr_bucket = (*b1);
	Bucket *temp_bucket;

	Bucket *new_bucket = malloc(sizeof(Bucket));
	if (new_bucket == NULL) {
		printf("Cannot allocate memory!\n");
		exit(1);
	}
	Initialize_Bucket(&new_bucket, ht->bucketentries);

	while (curr_bucket != NULL) {
		for (i = 0; i < curr_bucket->count; i++) {
			if (curr_bucket->voters[i]->PIN % (2 * ht->m) == ht->num_buckets - 1) {
				Insert_Bucket(b2, curr_bucket->voters[i], ht->bucketentries);
			}
			else {
				Insert_Bucket(&new_bucket, curr_bucket->voters[i], ht->bucketentries);
			}
		}
		free(curr_bucket->voters);
		temp_bucket = curr_bucket->next_bucket;
		free(curr_bucket);
		curr_bucket = temp_bucket;
	}
	(*b1) = new_bucket;
}

// Searches if there is a voter with a given PIN
int Search_HT(HTptr ht, int pin) {

	int j;
	int i1 = pin % ht->m;
	int i2 = pin % (2 * ht->m);
	Bucket *bucket1 = ht->buckets[i1];
	Bucket *bucket2 = ht->buckets[i2];

	while (bucket1 != NULL) {
		for (j = 0; j < bucket1->count; j++) {
			if (bucket1->voters[j]->PIN == pin) {
				return 1;
			}
		}
		bucket1 = bucket1->next_bucket;
	}
	while (bucket2 != NULL) {
		for (j = 0; j < bucket2->count; j++) {
			if (bucket2->voters[j]->PIN == pin) {
				return 1;
			}
		}
		bucket2 = bucket2->next_bucket;
	}

	return 0;
}

// Delete hash table, free memory
void Delete_HT(HTptr *ht) {

	int i, j;
	Bucket *bucket, *temp_bucket;
	Bucket **array_buckets = (*ht)->buckets;

	for (i = 0; i < (*ht)->num_buckets; i++) {
		bucket = array_buckets[i];
		while (bucket != NULL) {
			for (j = 0; j < bucket->count; j++) {
				free(bucket->voters[j]);
			}
			free(bucket->voters);
			temp_bucket = bucket->next_bucket;
			free(bucket);
			bucket = temp_bucket;
		}
	}
	free((*ht)->buckets);
	free(*ht);
}