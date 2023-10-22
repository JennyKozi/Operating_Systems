// 2D LIST

typedef struct node {
	struct node *next_voter;
	Voter *voter;
} ListNode;

typedef struct node_2d {
	int zipcode;
	int count; // number of voters (with the same zip) who have voted with
	struct node_2d *next_node;
	struct node *voters; // a list of voters (with the same zip) who have voted with
} ListNode_2d;

typedef struct node_2d *Listptr;

// Functions for main
void Create_List(Listptr *);
int Size_List(Listptr);
void Insert_List(Listptr *, Voter *);
void Print_Zip_List(Listptr, int);
void Print_Descending_List(Listptr);
void Delete_List(Listptr *);