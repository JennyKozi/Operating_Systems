// HASH TABLE

typedef struct bucket {
	int count;
	Voter **voters;
	struct bucket *next_bucket;
} Bucket;

typedef struct hash_table {
	int m;
	int p;
	int num_keys;
	int num_buckets;
	int bucketentries;
	Bucket **buckets;
} HT;

typedef struct hash_table *HTptr;

// Functions
void Create_HT(HTptr *, int);
void Insert_HT(HTptr, Voter *);
Voter* Search_HT(HTptr, int);
void Delete_HT(HTptr *);