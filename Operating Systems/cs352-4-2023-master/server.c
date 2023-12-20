/*
Project Name:  cs352-4-2023
Class:         CMPS 352
Filename:      server.c
Compile:       gcc -pthread -lcrypt -o server server.c LinkedList.c server-thread-2021.c authentication.c
Run:           ./server 17400
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h> // kill()
#include <semaphore.h>
#include "server-thread-2021.h"
#include "gomoku.h" // structs, macros, etc.

#define HOST "freebsd1.cs.scranton.edu"
#define SCOREBOARD_FILE "scoreboard.txt"


//Semaphore intiailizations
sem_t scoreboard_semaphore;
sem_t register_semaphore;

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
   //Memory is allocated here but never gets freed
   //An option here is to use atexit() to perform actions, like freeing,
   //after a program terminates.
   return message;
}

void sendBoard(int socket, char board[8][8]) {
   send(socket, board, (8*8), 0);
}

// ===== Player =====
void PLayerListPrint(List *this){ //internal server display
   ListFirst(this);
   Player *player = ListGetNext(this);
   printf("Scoreboard:\n");
   while (player != NULL) {
      printf("%s, %s, %d, %d, %d\n", player->name, player->email, player->wins, player->losses, player->ties);
      player = ListGetNext(this);
   }
}

Player* getPlayerByEmail(List *this, char* email) {
   Player *result = NULL;
   if(this->size == 0) { return result; }
   ListFirst(this);

   //returns Player object if email found
   while (this->cursor != NULL) {
      Player *currentPlayer = (Player *)this->cursor->item;
      if (strcmp(currentPlayer->email, email) == 0) {
         result = currentPlayer;
         return result; 
      }
      ListGetNext(this); 
   }
   return result; // Player not found
}

int emailAlreadyExists(List *this, char* email) { // boolean
   return getPlayerByEmail(this, email) != NULL;
}

Player* registerPlayer (int clientSocket){
   sem_wait(&register_semaphore);
   Player *player = (Player *)malloc(sizeof(Player));
   int status = 1;
     
   while(status) {
      player->email = receivePacket(clientSocket);
      
      if(emailAlreadyExists(scoreboard, player->email)){
         printf("User entered email that's been taken.");
      }else{
         status = 0;
      }
      sendInteger(clientSocket, status);
   }
   player->name = receivePacket(clientSocket);

   char *plainpswd = receivePacket(clientSocket);
   player->password = encode(plainpswd);
   player->wins = 0;
   player->losses = 0;
   player->ties = 0;
   ListInsert(scoreboard, player);
   sem_post(&register_semaphore);
   return player;
}

Player* loginPlayer(int clientSocket){
   Player *player = NULL;
   
   int loggedIn = 0;
   while(!loggedIn) {
      char *email = receivePacket(clientSocket);
      // send whether the email exists
      sendInteger(clientSocket, emailAlreadyExists(scoreboard, email));

      if(emailAlreadyExists(scoreboard, email)) {
         char *loginpswd = receivePacket(clientSocket);
         Player *findPlayer = getPlayerByEmail(scoreboard, email);
         if (authenticate(loginpswd, findPlayer->password)) {
            // point the result to the saved scoreboard player
            player = findPlayer;
            loggedIn = 1;
         }
      }
      sendInteger(clientSocket, loggedIn);
   }
   return player;
}

// ===== Game Board Manipulation =====
void setBoard(char board[8][8]){
   for(int y = 0; y < 8; y++){
      for(int x = 0; x < 8; x++){
         board[x][y] = EMPTY_SPACE;
      }
   }
}

void printBoard(char board[8][8]){ //internal server display
   for(int y = 0; y < 8; y++){
      for(int x = 0; x < 8; x++){
         printf("%c ", board[x][y]);
      }
      printf("\n");
   }
   printf("\n");
}

void placeStone(void *ptr1, void *ptr2){
   Game *game = (Game *)ptr1;
   InputResult *inputResult = (InputResult *)ptr2;
   game->board[inputResult->col][inputResult->row] = game->stone;
   game->x = inputResult->col;
   game->y = inputResult->row;
   game->nMoves++;
   printf("Stone Placed at (%d, %d)\n\n", inputResult->col, inputResult->row);
}
// ===== Player Input and Manipulation =====
int checkWin(void *gamePtr, void *gameContextPtr){
   Game *game = (Game *)gamePtr;
   GameContext *gameContext = (GameContext *)gameContextPtr;

   if(game->nMoves == 64) {
      game->status = GAME_STATUS_TIE;
   } //updates tie status

   if(game->status == GAME_STATUS_WINNER){
      sem_wait(&scoreboard_semaphore); // Wait for the semaphore
      if (gameContext->currentPlayerSocket == gameContext->socketPlayer1) {
         gameContext->player1->wins++;
         gameContext->player2->losses++;
      }
      else {
         gameContext->player1->losses++;
         gameContext->player2->wins++;
      }
      sem_post(&scoreboard_semaphore); // Release the semaphore after updating
      printf("\n");
      printf("%c is the winner!\n", game->stone);
      printBoard(game->board);
      printf("Game ended in %d moves.\n", game->nMoves);
      return 1;
   }
   else if(game->status == GAME_STATUS_TIE){
      sem_wait(&scoreboard_semaphore); // Wait for the semaphore
      gameContext->player1->ties++;
      gameContext->player2->ties++;
      sem_post(&scoreboard_semaphore); // Release the semaphore after updating
      printf("\n");
      printf("Game ends in a tie!\n");
      printBoard(game->board);
      return 1;
   }
   else{
      return 0;
   }
}

// ===== Board Checks =====
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
   if (count >= 5) {game->status = 1;}

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

void swapPlayer(void *gamePtr, void *gameContextPtr) {
   // Player 1 = White Stone. Player 2 = Black Stone
   Game *game = (Game *)gamePtr;
   GameContext *gameContext = (GameContext *)gameContextPtr;
   if (gameContext->currentPlayerSocket == gameContext->socketPlayer1) {
      gameContext->currentPlayerSocket = gameContext->socketPlayer2;
      game->stone = BLACK_STONE;
   }
   else {
      gameContext->currentPlayerSocket = gameContext->socketPlayer1;
      game->stone = WHITE_STONE;
   }
}

/* Verifys that the user's input is within the boundries
 * and that there is not a stone already placed here.
 */

int verifyInput(int col, int row, void *gamePtr) {
   if (row < 0 || col < 0 || row > 7 || col > 7) { 
      printf("(%d, %d) out of bounds\n", row, col);
      return INPUT_STATUS_OUT; 
   }
   Game *game = (Game *)gamePtr;
   if (game->board[col][row] == WHITE_STONE || game->board[col][row] == BLACK_STONE) {
      printf("(%d, %d) taken\n", row, col);
      return INPUT_STATUS_TAKEN;
   }
   return INPUT_STATUS_VALID;
}

void *runInputThread(void *gameContextPtr) {
   GameContext *gameContext = (GameContext *)gameContextPtr;
   InputResult *result = (InputResult*)malloc(sizeof(InputResult));
   result->status = INPUT_STATUS_VALID;
   result->row = -1;
   result->col = -1;
   
   if (gameContext->currentPlayerSocket == gameContext->socketPlayer1) {
      recv(gameContext->socketPlayer1, &result->row, sizeof(int), 0);
      recv(gameContext->socketPlayer1, &result->col, sizeof(int), 0);
   } 
   else {
      recv(gameContext->socketPlayer2, &result->row, sizeof(int), 0);
      recv(gameContext->socketPlayer2, &result->col, sizeof(int), 0);
   }
   return result;
}
// ===== Game Setup and Logic =====
void handleGame(void *gamePtr, void *gameContextPtr) {
   GameContext *gameContext = (GameContext *)gameContextPtr;
   Game *game = (Game *)gamePtr;
   game->status = GAME_STATUS_PLAYING;
   game->nMoves = 0;
   swapPlayer(game, gameContext); // let the white stone go first by default
   setBoard(game->board);

   // Greetings before a game start
   char message[MAX_PACKET_LENGTH];
   sprintf(message, "Player 1: %s, opponent: %s\n",gameContext->player1->name, gameContext->player2->name);
   sendPacket(gameContext->socketPlayer1, message);
   sprintf(message, "Player 2: %s, opponent: %s\n",gameContext->player2->name, gameContext->player1->name);
   sendPacket(gameContext->socketPlayer2, message);
   
   pthread_t inputThread, hThread, vThread;
   while (game->status == GAME_STATUS_PLAYING) {
      printBoard(game->board); // temporary
      // Sending the current board to respective players AND send game status
      if (gameContext->currentPlayerSocket == gameContext->socketPlayer1) {
         sendBoard(gameContext->socketPlayer1, game->board);
         sendInteger(gameContext->socketPlayer1, game->status);
      }else{
         sendBoard(gameContext->socketPlayer2, game->board);
         sendInteger(gameContext->socketPlayer2, game->status);
      }

      InputResult *inputResult;
      pthread_create(&inputThread, NULL, runInputThread, (void *)gameContext);
      pthread_join(inputThread, (void **)&inputResult);
      
      inputResult->status = verifyInput(inputResult->col, inputResult->row, game);
      
      // Send the input result to the client.
      if (gameContext->currentPlayerSocket == gameContext->socketPlayer1) {
         sendInteger(gameContext->socketPlayer1, inputResult->status);
      }
      else {
         sendInteger(gameContext->socketPlayer2, inputResult->status);
      }

      if (inputResult->status != INPUT_STATUS_VALID) {
         continue; // skip the rest of the loop
      }
      
      // Place the stone
      placeStone(game, inputResult);

      if (gameContext->currentPlayerSocket == gameContext->socketPlayer1) {
         sendBoard(gameContext->socketPlayer1, game->board);
      }else{
         sendBoard(gameContext->socketPlayer2, game->board);
      }

      // Check for horizontal and vertical wins.
      pthread_create(&hThread, NULL, horizontalCheck, (void*)game);
      pthread_create(&vThread, NULL, verticalCheck, (void*)game);

      // Wait for threads to finish
      pthread_join(hThread, NULL);
      pthread_join(vThread, NULL);

      // Check/update game status
      if(!checkWin(game, gameContext)) {
         swapPlayer(game, gameContext); // after move is made, swap player
      }
   }

   // send final game status to the clients
   sendBoard(gameContext->socketPlayer1, game->board);
   sendInteger(gameContext->socketPlayer1, game->status);
   sendBoard(gameContext->socketPlayer2, game->board);
   sendInteger(gameContext->socketPlayer2, game->status);
   if (game->status == GAME_STATUS_WINNER) {
      if (gameContext->currentPlayerSocket == gameContext->socketPlayer1) {
         // Player 1 won conditions
         sprintf(message, "You won and %s lost\n",gameContext->player2->name);
         sendPacket(gameContext->socketPlayer1, message);
         sprintf(message, "You lost and %s won\n",gameContext->player1->name);
         sendPacket(gameContext->socketPlayer2, message);
      }
      else {
         // Player 2 won conditions
         sprintf(message, "You won and %s lost\n",gameContext->player1->name);
         sendPacket(gameContext->socketPlayer2, message);
         sprintf(message, "You lost and %s won\n",gameContext->player2->name);
         sendPacket(gameContext->socketPlayer1, message);
      }
   }
   else {
      // Assuming this is a tie
      sendPacket(gameContext->socketPlayer1, "It was a draw");
      sendPacket(gameContext->socketPlayer2, "It was a draw");
   }

   // Send scoreboard to clients
   sprintf(message, "%s: %dW/%dL/%dT - %s: %dW/%dL/%dT", 
           gameContext->player1->name, gameContext->player1->wins, gameContext->player1->losses, gameContext->player1->ties,
           gameContext->player2->name, gameContext->player2->wins, gameContext->player2->losses, gameContext->player2->ties
   );
   sendPacket(gameContext->socketPlayer1, message);
   sendPacket(gameContext->socketPlayer2, message);
}

void *subserver(void *ptr) {
   printf("sub server created\n");
   GameContext *gameContext = (GameContext *)ptr;
   Game *game = (Game *)malloc(sizeof(Game));
   Player *player1;
   Player *player2;

   // handle login / register
   // Player 1
   int choice;
   receiveInteger(gameContext->socketPlayer1, &choice);
   if (choice == REGISTER) {
      player1 = registerPlayer(gameContext->socketPlayer1);
   }
   else if (choice == LOGIN) {
      player1 = loginPlayer(gameContext->socketPlayer1);
   }
   gameContext->player1 = player1;

   // Player 2
   receiveInteger(gameContext->socketPlayer2, &choice);
   if (choice == REGISTER) {
      player2 = registerPlayer(gameContext->socketPlayer2);
   }
   else if (choice == LOGIN) {
      player2 = loginPlayer(gameContext->socketPlayer2);
   }
   gameContext->player2 = player2;

   PLayerListPrint(scoreboard); //internal server display

   handleGame(game, gameContext);

   free(game);
   free(gameContext);
   pthread_exit(NULL);
}

void acceptConnections(int server_socket) {
   pthread_t subserver_thread;
   while (1) {
      int num_games = 0;
      printf("STARTING NEW GAME!\n\n\n");

      //Each game has 2 players/connections
      if (num_games < MAX_PENDING_CONNECTIONS / 2) {
         GameContext *gameContext = (GameContext *)malloc(sizeof(GameContext));
         gameContext->socketPlayer1 = accept_client(server_socket);
         printf("Player 1 connected.\n");

         gameContext->socketPlayer2 = accept_client(server_socket);
         printf("Player 2 connected.\n");

         pthread_create(&subserver_thread, NULL, subserver, (void *)gameContext);
         num_games++;
      }
   }
}

void saveScoreboardToFile(List *scoreboard) {
   sem_wait(&scoreboard_semaphore); // Wait for the semaphore
   FILE *file = fopen(SCOREBOARD_FILE, "w");
   if (file == NULL) {
      printf(" Error opening file for writing");
      sem_post(&scoreboard_semaphore); 
      return;
   }

   ListFirst(scoreboard);
   Player *player = ListGetNext(scoreboard);
   while (player != NULL) {
      fprintf(file, "%s %s %s %d %d %d\n", player->name, player->email, player->password, player->wins, player->losses, player->ties);
      player = ListGetNext(scoreboard);
   }

   fclose(file);
   printf(" Scoreboard saved to file!\n");
   sem_post(&scoreboard_semaphore); // Release the semaphore after file operation
}

void loadScoreboardFromFile(List *scoreboard) {
   FILE *file = fopen(SCOREBOARD_FILE, "r");
   if (file == NULL) {
      printf("Error opening file for reading");
      return;
   }

   char name[MAX_NAME_LENGTH];
   char email[MAX_EMAIL_LENGTH];
   char password[MAX_PASSWORD_LENGTH];
   int wins, losses, ties;

   sem_wait(&scoreboard_semaphore);

   while(fscanf(file, "%s %s %s %d %d %d\n", name, email, password, &wins, &losses, &ties) == 6){
      Player* player = (Player*)malloc(sizeof(Player));
      player->name = strdup(name);
      player->email = strdup(email);
      player->password = strdup(password);
      player->wins = wins;
      player->losses = losses;
      player->ties = ties;
      ListAppend(scoreboard, player);
   }
   fclose(file);
   sem_post(&scoreboard_semaphore);
}

void endServer() {
   saveScoreboardToFile(scoreboard);
   kill(getpid(),SIGINT);
   exit(1);
}

int main(int argc, char *argv[]) {
   int server_socket;
   server_socket = start_server(HOST, argv[1], 2);
   if (server_socket == -1) {
      printf("start server error\n");
      exit(1);
   }
   
   // Intercept the CTRL+C
   signal(SIGINT,endServer);

   // init semaphore with an initial value of 1
   sem_init(&scoreboard_semaphore, 0, 1);
   sem_init(&register_semaphore, 0, 1);

   // create scoreboard (defined in gomoku.h)
   scoreboard = ListCreate();
   loadScoreboardFromFile(scoreboard);

   acceptConnections(server_socket);

   close(server_socket);
} // end main

