/* File name:   scheduler-vailj2.c
   Description: A program hosted on the FreeBSD server
                that uses fork, execvp, waitpid, chdir, and other process
                management system calls that acts like a shell. This is the
                second assignemnt where we use POSIX threads to create worker
                threads. This program was extended to include scheduling of threads.
   Compile: gcc -pthread -o scheduler scheduler-vailj2.c LinkedList.c
   Run: ./thread
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <time.h>

#include "LinkedList.h"
#define MAX_ARG 64

typedef struct THREAD_VAL
{
    int argCount;
    char **arguments;
} ThreadVal;

List *jobList; // global jobList

void freeJob(Job *job)
{
    for (int i = 0; i < job->argCount; i++)
    {
        free(job->command[i]);
    }
    free(job);
}

char *getInput()
{
    char *input = NULL;
    size_t bufsize = 0;
    getline(&input, &bufsize, stdin);
    return input;
}

int parseToken(char *userTok, char *arguments[])
{
    int count = 0;
    while (userTok != NULL && count < (MAX_ARG - 1))
    {
        arguments[count] = userTok;
        userTok = strtok(NULL, " \t\n");
        count++;
    }
    arguments[count] = NULL;
    return count;
}

int terminalInput(char *arguments[])
{
    char *userInput = getInput();
    return parseToken(strtok(userInput, " \t\n"), arguments);
}
//prints all jobs in list with info
void printJobs()
{
    ListFirst(jobList);
    printf("Job List:\n");
    printf("\n");
    while (1)
    {
        Job *job = ListGetNext(jobList);
        if (job == NULL)
        {
        break; 
        }
        printf("Job ID: %p\n", (void *)job); //prints job id
        printf("Submit Time: %ld\n", job->submitTime);
        printf("Start Time: %ld\n", job->startTime);
        printf("Command: ");
        for (int i = 0; i < job->argCount; i++)
        {
        printf("%s ", job->command[i]);
        }
        printf("\n\n");
    }
}

void executeCMD(ThreadVal *paramPtr)
{
    if (execvp(paramPtr->arguments[0], paramPtr->arguments) == -1)
    {
        printf("Error:");
        for (int i = 0; i < paramPtr->argCount; i++)
        {
        printf(" %s", paramPtr->arguments[i]);
        }
        printf("\n");
        exit(1);
    }
}

Job *createJob()
{
    int argCount;
    char *arguments[MAX_ARG];
    argCount = terminalInput(arguments);
    Job *jobPtr = (Job *)malloc(sizeof(Job));
    printf("Enter start time: ");
    scanf("%ld", &jobPtr->startTime);
    jobPtr->submitTime = time(NULL);
    jobPtr->argCount = argCount;
    for (int i = 0; i < 5; i++) {
        jobPtr->command[i] = NULL;
    }
    for (int i = 0; i < argCount; i++) //copies arguments
    {
        jobPtr->command[i] = strdup(arguments[i]);
    }
    jobPtr->command[argCount] = NULL;
    return jobPtr;
}
//scheduler thread, checks for input (in charge of calling server thread)
void *schedulerThread(void *arg)
{
    char input;
    while (1)
    {
        printf("Enter option (+ <command>, p, -)\n");
        printf("%s: ", getlogin());
        scanf(" %c", &input); // Read a single character, ignore leading whitespace

        switch (input)
        {
        case '+': //add job clause
            printf("Adding a job...\n");
            Job *job = createJob();
            ListInsert(jobList, job);
            printf("Job added successfully.\n");
            printf("\n");
            break;

        case 'p': //print job clause
            printJobs();
            break;

        case '-': //delete job clause
            printf("\n");
            Job *deletedJob = ListDelete(jobList);
            if (deletedJob != NULL)
            {
                freeJob(deletedJob);
                printf("Job deleted.\n");
            }
            else
            {
                printf("No job to delete.\n");
                printf("\n");
            }
            break;
        //in case for invalid command.
        default:
            printf("Invalid command. Try again.\n");
        }
    }
}
void *workerThread(void *arg)
{
    ThreadVal *paramPtr = (ThreadVal *)arg;
    int pid, status;
    pid = fork();
    if (pid < 0)
    {
        perror("fork");
    }
    else if (pid == 0)
    {
        executeCMD(paramPtr);
    }
    else
    {
        waitpid(pid, &status, 0);
        if (WIFEXITED(status))
        {
            printf("Job Return Code: %d\n", WEXITSTATUS(status));
        }
    }
    for (int i = 0; i < paramPtr->argCount; i++)
    {
        free(paramPtr->arguments[i]);
    }
    free(paramPtr->arguments);
    free(paramPtr);
    pthread_exit(NULL);
}
//was in main last assignment, now separated into server thread.
//constantly checks jobList.
void *serverThread(void *arg)
{
    while (1)
    {
        if (jobList->size==0)
        {
            sleep(1);
            continue;
        }

        Job *nextJob = ListGetFirst(jobList); //checks next (first) job
        long currentSystemTime = time(NULL);
        if ((currentSystemTime >= nextJob->submitTime + nextJob->startTime))
        {
            ListDelete(jobList);

            pthread_t workerID;
            ThreadVal *paramPtr = (ThreadVal *)malloc(sizeof(ThreadVal));

            paramPtr->argCount = nextJob->argCount;
            paramPtr->arguments = 
            (char **)malloc((nextJob->argCount + 1) * sizeof(char *));

            for (int i = 0; i < nextJob->argCount; i++)
            {
                paramPtr->arguments[i] = strdup(nextJob->command[i]); //duplicate arguments
            }
            paramPtr->arguments[nextJob->argCount] = NULL;
            pthread_create(&workerID, NULL, workerThread, (void *)paramPtr);
            freeJob(nextJob);
        }
        sleep(1);
    }
}

int main()
{
    jobList = ListCreate();
    pthread_t schedulerID, serverID;
    pthread_create(&schedulerID, NULL, schedulerThread, NULL);
    pthread_create(&serverID, NULL, serverThread, NULL);

    pthread_join(schedulerID, NULL);
    pthread_join(serverID, NULL);

    ListDestroy(jobList);
    return 0;
}
