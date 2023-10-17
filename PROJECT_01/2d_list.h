// 2D LIST

typedef struct node {
	struct node *next_voter;
	Voter *voter;
} ListNode;

typedef struct node_2d {
	int zipcode;
	struct node_2d *next_node;
	struct node *voters;
} ListNode_2d;

typedef struct node_2d *Listptr;

// Functions
void Create_List(Listptr *);
int Size_List(Listptr);
void Insert_List(Listptr *, Voter *);
void Print_Zip_List(Listptr, int);
void Delete_List(Listptr *);