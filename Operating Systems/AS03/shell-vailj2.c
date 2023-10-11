/* File name:   shell-vailj2.c
   Description: A program hosted on the FreeBSD server
                that uses fork, execvp, waitpid, chdir, and other process
                management system calls that acts like a shell.
   Compile: gcc -o shell shell-vailj2.c
   Run: ./shell
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARG 64

char* user_input(){
    char* input = NULL;
    //From c library 'cplusplus.com' getline is better than fgets and scanf
    //as it takes in a string and dynamically allocates mem (with type size_t)
    size_t bufsize = 0;
    getline(&input, &bufsize, stdin); //stdin for standard input for getline
    return input;
}
int parseToken(char *userTok, char *arguments[]){
    int count = 0;
    while(userTok != NULL && count < (MAX_ARG - 1)){
        arguments[count] = userTok;
        userTok = strtok(NULL, " \t\n");
        count++;
    }
    // Null terminated string for easier reading
    arguments[count] = NULL;
    return count;
}
int main() {
    char* input;
    char* userTok;
    char* arguments[MAX_ARG];
    int argCount;
    int pid;
    int wait;
while (1){
printf("%s: ", getlogin());
input = user_input();
userTok = strtok(input, " \t\n");
argCount = parseToken(userTok, arguments); //parse arguments : get count
if(argCount > 0){ //CHECK USER CMD's
if(strcmp(arguments[0], "cd") == 0 || strcmp(arguments[0], "chdir") == 0){ //DIRECTORY CHANGE
    if(argCount >= 2){ //Check other arguments
        if(chdir(arguments[1]) == -1){ //checks to see if error
        perror("chdir"); //prints dedicated error
        }
    }else{
        printf("Usage: cd 'directory'\n"); // IFF num arguments missed
    }
}else if(strcmp(arguments[0], "exit") == 0){ //EXIT CODE
    return 0;
}else{
    pid = fork();
    if(pid < 0){ //check for error
        perror("fork");
    }
    else if(pid == 0){
        if(execvp(arguments[0], arguments) == -1){
        perror("execvp");
        return 0;
        }
    }else{
        waitpid(pid, &wait, 0); //wait for child process
    }
}
    free(input); //free memory
}
}
}

