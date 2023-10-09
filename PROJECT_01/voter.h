// VOTER

typedef struct voter {
	int PIN;
	char *name;
	char *last_name;
	int zipcode;
	char has_voted;
} Voter;

typedef struct voter *Voterptr;