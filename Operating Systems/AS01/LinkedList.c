/* File name:   LinkedList.c
   Descirption: the implementation of LinkedList
*/
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.h"
 
List *ListCreate() {
   List *list = (List*) malloc(sizeof(List));
   list->first = NULL;
   list->last = NULL;
   list->size = 0;
   list->cursor = NULL;
   return list;
}
 
//NOTE: this function may cause memory leaks
//      if the item in a node is linked to any data on the heap
//      the data would not freed by this function.
void ListDestroy(List *this) {
   Node *node = this->first;
   while (node != NULL) {
      Node *next = node->next;
      printf("item@%p, node@%p to be freed\n", node->item, node);
      free(node->item);
      free(node);
      node = next;
   }
   free(this);
}
        
Node *NodeCreate(void *item) {
   Node *node = (Node*) malloc(sizeof(Node));
   node->item = item;
   node->next = NULL;
   return node;
}
 
void ListInsert(List *this, void* item) {
   Node *node = NodeCreate(item);
   node->next = this->first;
   this->first = node;
   if (this->size == 0) {
      this->last = node;
   }
   this->size++;
}
 
void ListFirst(List *this) {
   this->cursor = this->first;
}
 
void *ListGetNext(List *this) {
   void *result = NULL;
   if (this->cursor != NULL) {
      result = this->cursor->item;
      this->cursor = this->cursor->next;
   }
   return result;
}

// allows the user to add a node at the end of the list.
void ListAppend(List *this, void* item){
   Node *node = NodeCreate(item); //creates the node
   // sets the current last node's next to point at new node
   this->last->next = node;
   // sets the new node to be last
   this->last = node;
   // checks to see if list is empty.
   // if it is, new node is first and last in list.
   if (this->size == 0) {
      this->last = node;
      this->first = node;
   }
   // updates size regardless
   this->size++;    
}

// deletes the first node if exists & returns its item
// if list is empty, returns NULL in JobManager.c
void *ListDelete(List *this){
   // creates new node so we can access 2nd node
   Node *node = this->first;
   // makes second node the first.
   this->first = node->next;
   // frees both the node and the item
   free(node->item);
   free(node); 
   // update size
   this->size--;
}

// returns the item at index if exists, otherwise NULL;
// the first item in the list has the index of 0. 
void *ListGetAt(List *this, int index){
   // initialize counter for while loop
   int counter = 0;
   // starts cursor at first node
   // so its ready to iterate
   // for bigger lists, maybe add a little sorting
   // method to see if we can start the cursor in
   // the middle or end of list (closer to target index)
   this->cursor = this->first;
   // while loop continues until hitting index
   while(counter != index){
      // iterates cursor
      this->cursor = this->cursor->next;
      // for while condition
      counter++;
   }
   //returns cursors item
   return this->cursor->item;
}