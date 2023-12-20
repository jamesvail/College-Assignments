/*
Project Name:  cs352-4-2023
Class:         CMPS 352
Filename:      client.c
Compile:       gcc -pthread -lcrypt -o client client.c LinkedList.c client-thread-2021.c authentication.c
Run:           ./client freebsd1.cs.scranton.edu 17400
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "client-thread-2021.h"
#include "gomoku.h" // structs, macros, etc.

//print updated board, postgame
void printBoard(char board[8][8]){
   for(int y = 0; y < 8; y++){
      for(int x = 0; x < 8; x++){
         printf("%c ", board[x][y]);
      }
      printf("\n");
   }
   printf("\n");
}

// ===== Socket Controls =====
void sendInteger(int socket, int num) {
   send(socket, &num, sizeof(int), 0);
}

void receiveInteger(int socket, int *num) {
   recv(socket, num, sizeof(int), 0);
}

void sendPacket(int socket, char *message) {
   int nBytes = strlen(message) + 1;
   send(socket, &nBytes, sizeof(int), 0);
   send(socket, message, nBytes, 0);
}

char* receivePacket(int socket) {
   int nBytes = 0;
   recv(socket, &nBytes, sizeof(int), 0);
   char *message = (char*)malloc(nBytes);
   recv(socket, message, nBytes, 0);
   return message;
}

void setupPlayer(int server_socket) {
   char email[MAX_EMAIL_LENGTH];
   char name[MAX_NAME_LENGTH];
   char *plainpswd = (char*) malloc(MAX_PASSWORD_LENGTH);
   int choice = -1;
   
   while (choice != REGISTER && choice != LOGIN) {
      printf("1. Register\n2. Login\nEnter your choice: ");
      scanf("%d", &choice);
      getchar(); //consume newl
      if (choice == REGISTER) {
         sendInteger(server_socket, choice);
         int notValidEmail = 1;
         while(notValidEmail){
            printf("Enter your email: ");
            fgets(email, sizeof(email), stdin);
            email[strcspn(email, "\n")] = '\0';
            sendPacket(server_socket, email);
            receiveInteger(server_socket, &notValidEmail);
            if(notValidEmail) {printf("Email already registered.\n");}
         }
                  
         getpasswd(plainpswd, MAX_PASSWORD_LENGTH); //auto prints prompt
         printf("\n");
         
         printf("Enter your name: ");
         fgets(name, sizeof(name), stdin);
         name[strcspn(name, "\n")] = '\0';

         // Sending email, password, and name to the server for registration
         sendPacket(server_socket, name);
         sendPacket(server_socket, plainpswd);
         free(plainpswd); 
      }
      else if (choice == LOGIN) {
         sendInteger(server_socket, choice);
         int validEmail = 0;
         int loggedIn = 0;
         while(!loggedIn){
            printf("Enter your email: ");
            fgets(email, sizeof(email), stdin);
            email[strcspn(email, "\n")] = '\0';
            sendPacket(server_socket, email);
            // receive whether the email exists
            receiveInteger(server_socket, &validEmail);
            if (validEmail) {
               getpasswd(plainpswd, MAX_PASSWORD_LENGTH);
               printf("\n");
               sendPacket(server_socket, plainpswd);
            }
            // receive if the user is logged in or not
            receiveInteger(server_socket, &loggedIn);

            if(!loggedIn) {printf("Incorrect email or password.\n");}
         }
                  
         free(plainpswd); //free after we dont use it
      }
      else {
         printf("Invalid Choice.");
      }
   }
}

int main(int argc, char* argv[]) {
   char board[8][8];
   
   // Connect to the game
   int server_socket = get_server_connection(argv[1], argv[2]);
   printf("Connected to Game.\n");

   // handle login / register
   setupPlayer(server_socket);
   printf("------------------------------------------\n");

   printf("%s\n", receivePacket(server_socket)); // Start game greeting
   
   // Main game loop
   int game_status = GAME_STATUS_PLAYING;
   while (game_status == GAME_STATUS_PLAYING) {
      // Receive and display the current board
      recv(server_socket, board, (8*8), 0);
      printBoard(board);

      receiveInteger(server_socket, &game_status);
      if (game_status == GAME_STATUS_PLAYING) {
         int row, col;
         printf("Enter your move (row column): ");
         scanf("%d %d", &row, &col);

         // Send the move to the server
         send(server_socket, &row, sizeof(row), 0);
         send(server_socket, &col, sizeof(col), 0);
      
         // Update the input status received from the server.
         int input_status;
         receiveInteger(server_socket, &input_status);
         if (input_status == INPUT_STATUS_OUT) {
            printf("(%d, %d) out of bounds\n", row, col);
         }
         else if (input_status == INPUT_STATUS_TAKEN) {
            printf("(%d, %d) taken\n", row, col);
         }

         recv(server_socket, board, (8*8), 0);
         printBoard(board);
         printf("+++++++++++++++++\n");
         printf("+++++++++++++++++\n\n");
      }
      else if (game_status == GAME_STATUS_WINNER) {
         printf("\n");
      }
      else if (game_status == GAME_STATUS_TIE) {
         printf("\n");
      }
      else {
        printf("Invalid game status %d\n\n",game_status);
      }
   }
   printf("%s", receivePacket(server_socket)); // End of game WIN/TIE message
   printf("%s\n", receivePacket(server_socket)); // Print out scoreboard

   close(server_socket);
}
