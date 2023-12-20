/*
Project Name:  cs352-4-2023
Class:         CMPS 352
Filename:      asgn6.c
Compile:       gcc -pthread -o asgn6 asgn6.c
Run:           ./asgn6
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include "client-thread-2021.h"
#include "server-thread-2021.h"

#define SERVER_ADDRESS "freebsd1.cs.scranton.edu"
#define SERVER_PORT  17400

#define GAME_STATUS_PLAYING 0
#define GAME_STATUS_WINNER 1
#define GAME_STATUS_TIE 2

#define WHITE_STONE 'W'
#define BLACK_STONE 'B'
#define EMPTY_SPACE '-'

typedef struct GAME {
   int nMoves;      // total number of valid moves
   int status;      // 0: playing, 1: a winner, 2: a tie
   char stone;      // Current Player. 'W': for white stone, 'B' for black
   int x, y;        // (x, y): current move. x=col, y=row
   char board[8][8];// [x][y] AKA [col][row]
} Game;

typedef struct INPUT_RESULT {
   int status;
   int row;
   int col;
} InputResult;

// ==========  James   ==========
//deploys empty board
void setBoard(char board[8][8]){
   for(int y = 0; y < 8; y++){
      for(int x = 0; x < 8; x++){
         board[x][y] = EMPTY_SPACE;
      }
   }
}
//print updated board
void printBoard(char board[8][8]){
   for(int y = 0; y < 8; y++){
      for(int x = 0; x < 8; x++){
         printf("%c ", board[x][y]);
      }
      printf("\n");
   }
   printf("\n");
}
int checkWin(void *ptr){
   Game *game = (Game *)ptr;
   if(game->nMoves == 64) {game->status = GAME_STATUS_TIE;} //updates tie status

   if(game->status == GAME_STATUS_WINNER){
      printf("\n");
      printf("%c is the winner!\n", game->stone);
      printBoard(game->board);
      printf("Game ended in %d moves.\n", game->nMoves);
      return 1;
   }else if(game->status == GAME_STATUS_TIE){
      printf("\n");
      printf("Game ends in a tie!\n");
      printBoard(game->board);
      return 1;
   }else{
      return 0;
   }
}
//consolidated main code into a function.
void placeStone(void *ptr1, void *ptr2){
   Game *game = (Game *)ptr1;
   InputResult *inputResult = (InputResult *)ptr2;
   game->board[inputResult->col][inputResult->row] = game->stone;
   game->x = inputResult->col;
   game->y = inputResult->row;
   game->nMoves++;
   printf("Stone Placed at (%d, %d)\n\n", inputResult->col, inputResult->row);
}  

// ==========   Jay    ==========
void *horizontalCheck(void *ptr) {
   Game *game = (Game *)ptr;
   int count = 1; // Initialize count to 1 for the current stone placed

   for (int x = game->x - 1; x >= 0; x--) { // Check to the left
      if (game->board[x][game->y] == game->stone) {
         count++;
      } else {
         break;
      }
   }
   for (int x = game->x + 1; x < 8; x++) { // Check to the right
      if (game->board[x][game->y] == game->stone) {
         count++;
      } else {
         break;
      }
   }
   if (count >= 5) {game->status = GAME_STATUS_WINNER;}

   pthread_exit(NULL);
}

void *verticalCheck(void *ptr) {
   Game *game = (Game *)ptr;
   int count = 1; // Initialize count to 1 for the current stone placed

   for (int y = game->y - 1; y >= 0; y--) { // Check Downwards
      if (game->board[game->x][y] == game->stone) {
         count++;
      } else {
         break;
      }
   }
   for (int y = game->y + 1; y < 8; y++) { // Check Upwards
      if (game->board[game->x][y] == game->stone) {
         count++;
      } else {
         break;
      }
   }
   if (count >= 5) {game->status = GAME_STATUS_WINNER;} 

   pthread_exit(NULL);
}

// ========== Jonathan ==========
void printInputPrompt(void *gamePtr) {
   Game *game = (Game *)gamePtr;
   if (game->stone == WHITE_STONE) {
      printf("[WHITE] ");
   }
   else {
      printf("[BLACK] ");
   }
   printf("Enter Row# Column#: ");
}

void swapPlayer(void *gamePtr) {
   Game *game = (Game *)gamePtr;
   if (game->stone == WHITE_STONE) {
      game->stone = BLACK_STONE;
   }
   else {
      game->stone = WHITE_STONE;
   }
}

/* Verify that the user's input is within the boundries
 * and that there is not a stone already placed here.
 */
int verifyInput(int col, int row, void *gamePtr) {
   if (row < 0 || col < 0 || row > 7 || col > 7) { 
      printf("(%d, %d) out of bounds\n", row, col);
      return 0; 
   }
   Game *game = (Game *)gamePtr;
   if (game->board[col][row] == WHITE_STONE || game->board[col][row] == BLACK_STONE) {
      printf("(%d, %d) taken\n", row, col);
      return 0;
   }
   return 1;
}

void *runInputThread(void *gamePtr) {   // thread function
   Game *game = (Game *)gamePtr;
   InputResult *result = (InputResult*)malloc(sizeof(InputResult));
   result->status = 0;
   result->row = -1;
   result->col = -1;

   printInputPrompt(game);
   if (scanf("%d %d", &result->row, &result->col) != 2) {
      printf("Invalid input. Please enter two integers separated by a space.\n");
      return result;
   }

   if (!verifyInput(result->col, result->row, game)) {
      return result;
   }

   result->status = 1;
   return result;
}

void *clientConn(void *arg) {
   int *clientSocketPtr = (int *)arg;
    int client_socket = *clientSocketPtr;
    char buffer[1024];
    int bytes_received;

    while (1) {
      if (recv(client_socket, buffer, sizeof(buffer), 0) <= 0) {
         printf("Client disconnected.\n");
         break;
        }
        printf("Received from client: %s", buffer);

        send(client_socket, buffer, bytes_received, 0);
    }

    // Close the client socket
    close(client_socket);

    // Exit the thread
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
   Game *game = (Game *)malloc(sizeof(Game));
   game->status = GAME_STATUS_PLAYING;
   swapPlayer(game); // let the white stone go first by default
   setBoard(game->board);
   
   int serverSocket = start_server(SERVER_ADDRESS, argv[1], 2);

   int player1Socket = accept_client(serverSocket);
   int player2Socket = accept_client(serverSocket);
   pthread_t player1Thread, player2Thread;
   pthread_create(&player1Thread, NULL, clientConn, (void *)&player1Socket);
   pthread_create(&player2Thread, NULL, clientConn, (void *)&player2Socket);

   pthread_t inputThread, hThread, vThread;
   while (game->status == GAME_STATUS_PLAYING) {
      printBoard(game->board);
      
      InputResult *inputResult;
      pthread_create(&inputThread, NULL, runInputThread, (void *)game);
      pthread_join(inputThread, (void **)&inputResult);
      if (!inputResult->status) { continue; } // if input is bad, skip this loop iteration

      // Place the stone
      placeStone(game, inputResult);

      // do the checks for horizontal and vertical wins.
      pthread_create(&hThread, NULL, horizontalCheck, (void*)game);
      pthread_create(&vThread, NULL, verticalCheck, (void*)game);

      //Wait for threads to finish
      pthread_join(hThread, NULL);
      pthread_join(vThread, NULL);

      //checks/updates game status
      if(!checkWin(game)){
         swapPlayer(game); // after move is made, swap player
      }
      //free input result
      free(inputResult);
   }
   free(game);
} // end main
