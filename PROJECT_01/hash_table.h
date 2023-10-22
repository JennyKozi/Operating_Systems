// HASH TABLE

typedef struct bucket {
	int count; // Number of voters
	Voter **voters; // Array of pointers to voters
	struct bucket *next_bucket; // Pointer to overflow bucket
} Bucket;

typedef struct hash_table {
	int m; // Modulo for hash function (m and 2*m)
	int p; // Pointer to the next bucket for split
	int num_keys; // Number of voters there are in the table
	int num_buckets; // Number of non-overflow buckets
	int bucketentries; // Max number of entries a bucket can have
	Bucket **buckets; // Array of pointers to buckets
} HT;

typedef struct hash_table *HTptr;

// Functions for main
void Create_HT(HTptr *, int);
void Insert_HT(HTptr, Voter *);
Voter* Search_HT(HTptr, int);
void Delete_HT(HTptr *);