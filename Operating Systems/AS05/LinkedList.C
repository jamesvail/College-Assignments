/* File name: LinkedList.c
   Description: Implementation of LinkedList
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkedList.h"

List *ListCreate()
{
    List *list = (List *)malloc(sizeof(List));
    list->first = NULL;
    list->last = NULL;
    list->size = 0;
    list->cursor = NULL;
    return list;
}

void ListDestroy(List *this)
{
    Node *node = this->first;
    while (node != NULL)
    {
        Node *next = node->next;
        free(node->item);
        free(node);
        node = next;
    }
    free(this);
}

Node *NodeCreate(void *item)
{
    Node *node = (Node *)malloc(sizeof(Node));
    node->item = item;
    node->next = NULL;
    return node;
}

// Update the ListInsert function
void ListInsert(List *this, void *item)
{
    Node *node = NodeCreate(item);
    node->next = NULL;

    Job *job = (Job *)item;

    if (this->size == 0 || (job->submitTime + job->startTime) <= ((Job *)(this->first->item))->submitTime + ((Job *)(this->first->item))->startTime)
    {
        // beginning of list
        node->next = this->first;
        this->first = node;
    }
    else
    {
        // find position
        Node *prev = NULL;
        Node *current = this->first;

        while (current != NULL && (job->submitTime + job->startTime) > ((Job *)(current->item))->submitTime + ((Job *)(current->item))->startTime)
        {
            prev = current;
            current = current->next;
        }
        prev->next = node;
        node->next = current;
    }

    //updates
    if (node->next == NULL)
    {
        this->last = node;
    }

    this->size++;
}


void ListAppend(List *this, void *item)
{
    Node *node = NodeCreate(item);
    node->next = NULL;

    if (this->size == 0)
    {
        // Empty list, set as both first and last
        this->first = node;
        this->last = node;
    }
    else
    {
        // Append to the end of the list
        this->last->next = node;
        this->last = node;
    }

    this->size++;
}
void *ListGetNext(List *this) {
   void *result = NULL;
   if (this->cursor != NULL) {
      result = this->cursor->item;
      this->cursor = this->cursor->next;
   }
   return result;
}
void *ListDelete(List *this)
{
    if (this->size == 0)
    {
        // Empty list
        return NULL;
    }

    Node *node = this->first;
    this->first = node->next;

    // Update the last pointer if the list becomes empty
    if (this->first == NULL)
    {
        this->last = NULL;
    }

    void *item = node->item;
    free(node);
    this->size--;

    return item;
}

void *ListGetAt(List *this, int index)
{
    int counter = 0;
    this->cursor = this->first;

    while (counter != index && this->cursor != NULL)
    {
        this->cursor = this->cursor->next;
        counter++;
    }

    return (this->cursor != NULL) ? this->cursor->item : NULL;
}

void ListFirst(List *this) {
   this->cursor = this->first;
}

//returns a job, gets first item in list.
Job *ListGetFirst(List *this) {
    if (this->first != NULL) {
        return (Job *)this->first->item;
    }
    return NULL;
}
