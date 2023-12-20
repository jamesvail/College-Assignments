/* File name:   LinkedList.c
   Descirption: the implementation of LinkedList
*/

#include "LinkedList.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_JOB_ARGS 5

struct JOB {
   char *command[MAX_JOB_ARGS]; 
   long submitTime; // domain = { positive long integer, seconds }
   long startTime;  // domain = { positive long integer, seconds }
};
typedef struct JOB Job;

List *ListCreate() {
  List *list = (List *)malloc(sizeof(List));
  list->first = NULL;
  list->last = NULL;
  list->size = 0;

  list->cursor = NULL;
  return list;
}

// NOTE: this function may cause memory leaks
//       if the item in a node is linked to any data on the heap
//       the data would not freed by this function.
void ListDestroy(List *this) {
  Node *node = this->first;
  while (node != NULL) {
    Node *next = node->next;
    // printf("item@%p, node@%p to be freed\n", node->item, node);
    free(node->item);
    free(node);
    node = next;
  }
  free(this);
}

Node *NodeCreate(void *item) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->item = item;
  node->next = NULL;

  return node;
}

void ListInsert(List *this, void *item) {
  Node *node = NodeCreate(item);

  node->next = this->first;
  this->first = node;
  if (this->size == 0) {
    this->last = node;
  }
  this->size++;
}

void ListFirst(List *this) { this->cursor = this->first; }

void *ListGetNext(List *this) {
  void *result = NULL;
  if (this->cursor != NULL) {
    result = this->cursor->item;
    this->cursor = this->cursor->next;
  }
  return result;
}

void ListAppend(List *this, void *obj) {
  Node *node = NodeCreate(obj);
  /* If this is not the first node, then we have to point
     the NEXT of the current LAST node to the new node. */
  if (this->size > 0) {
    this->last->next = node;
  } else {
    this->first = node;
  }
  this->last = node;
  this->size++;
}

void *ListDelete(List *this) {
  // Keep track of the value being deleted in order to return
  Node *node = this->first;
  if (node != NULL) {
    void *item = node->item;
    Node *next = node->next;
    free(node);
    this->first = next;
    this->size--;
    return item;
  } else {
    return NULL;
  }
}

void *ListGetAt(List *this, int index) {
  if (this->size == 0) {
    return NULL;
  }

  Node *node = this->first;
  int i = 0;
  while (node != NULL) {
    if (i == index) {
      return node->item;
    }
    node = node->next;
    i++;
  }

  /* If we get to this point, the index does not exist in the
     list, therefore just return NULL */
  return NULL;
}
