/* File name:   LinkedList.h
   Description: the header file for LinkedList. 
                #include "LinkedList.h" in your program. 
*/

#define MAX_ARG 64

//updated job structure
struct JOB {
    char *command[5];
    long submitTime;
    long startTime;
    int argCount;
    struct JOB *next;
};
typedef struct JOB Job;

// node structure
struct NODE {
   void *item;
   struct NODE *next;
};
// give "struct NODE" a short name (Node)
typedef struct NODE Node;

// define a struct and give it a short name
typedef struct LIST {
   Node *first;
   Node *last;
   int size;
   Node *cursor;
}List; 

List *ListCreate();

void ListDestroy(List *this);

Node *NodeCreate(void *item);

void ListInsert(List *this, void* item);

void ListFirst(List *this);

void *ListGetNext(List *this);

void ListAppend(List *this, void* item);

void *ListDelete(List *this);

void *ListGetAt(List *this, int index);

Job *ListGetFirst(List *this);
