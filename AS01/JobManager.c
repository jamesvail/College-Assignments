/* File name:   JobManager.c
   Description: A job manager application that allows the
                 user to create and manage a linked list.
                 The user, when prompted for input, can
                 either insert, append, delete, or 
                 retrieve from the list (with codes 1-4
                 respeectively). When done, the user can
                 enter 99 to delete the list and end the
                 program. Program needs to be ran with -w
                 to ignore warnings from delete function in
                 LinkedList.c
   Compile: gcc -o jobs LinkedList.c JobManager.c -w
   Run: ./jobs
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LinkedList.h"

struct JOB {
   int id;
   int type;
   char message[25];
}; typedef struct JOB Job;

void JobPrint(Job *job) {
   // prints correct format of node
   printf("[%d, %d, %s]\n", job->id, job->type, job->message);
}

void JobListPrint(List *this) {
   ListFirst(this);
   Job *job = ListGetNext(this); 
   while (job != NULL) {
      JobPrint(job);
      job = ListGetNext(this);
   }
}

Job *JobCreate(int id,int type, char *message) { 
   Job *job = (Job*)malloc(sizeof(Job));
   //updates id
   job->id = ++id;
   //updates type
   job->type = type;
   strcpy(job->message, message);
   //print function inside of jobcreate, less code
   JobPrint(job);
   //returns created job for list functions
   return job;
}

int main() {
   // creates new list and initiates some 
   // variables that aid with managing list
   // id, type, message, code (for user)
   List *jobs = ListCreate();
   int id = 0;
   int type;
   char message[25];
   int code = 0;
   // while loop that continues until user enters 99
   // if statements for every code
   while(code != 99){
   // First scans the first input so program knows whether
   // or not to need more user input.
   scanf("%d", &code);
   if(code == 1){ // insert code
      // 2 inputs for insert
      scanf("%d%s", &type, message);
      printf("INSERTED: "); ListInsert(jobs, JobCreate(id, type, message));
      // updates id
      id++;
   }else if(code == 2){ // append code
      // 2 inputs for append
      scanf("%d%s", &type, message);
      printf("APPENDED: "); ListAppend(jobs, JobCreate(id, type, message));         
      // updates id
      id++;
   }else if(code == 3){ // delete code
      // no further input needed
      printf("DELETED: ");
      // checks to see if list is empty
      if(jobs->size == 0){
      printf("NULL\n");   
      }else{
      // prints job before deleting it. I found using ListGetAt to return 
      // the job was best so that I could free the memory in the ListDelete
      // function.
      JobPrint(ListGetAt(jobs, 0));
      ListDelete(jobs);}
   }else if(code == 4){ // retrieve code
      // retrieve only needs index input (named type)
      scanf("%d", &type);
      printf("RETRIEVED: ");
      // makes sure user cant go past list size. Removes seg faults.
      if(type >= jobs->size){
      printf("NULL\n");   
      }else{
      JobPrint(ListGetAt(jobs, type));}
   }else if(code == 99){ // end code
      // when user wishs to end, prints all nodes
      JobListPrint(jobs);}
   } //Wasn't sure if comments count so I cut while loop to 40 char.
}