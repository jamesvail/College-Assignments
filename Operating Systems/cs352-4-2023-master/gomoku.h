#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "LinkedList.h"
#include "authentication.h"

#define GAME_STATUS_PLAYING 0
#define GAME_STATUS_WINNER 1
#define GAME_STATUS_TIE 2

#define INPUT_STATUS_OUT 0
#define INPUT_STATUS_TAKEN 1
#define INPUT_STATUS_VALID 2

#define REGISTER 1
#define LOGIN 2

#define WHITE_STONE 'W'
#define BLACK_STONE 'B'
#define EMPTY_SPACE '-'

#define MAX_PACKET_LENGTH 254
#define MAX_NAME_LENGTH 15
#define MAX_EMAIL_LENGTH 50
#define MAX_PASSWORD_LENGTH 50

#define MAX_PENDING_CONNECTIONS 20

typedef struct PLAYER {
   char *email;
   char *password;  // encoded
   char *name;
   int wins;
   int losses;
   int ties;
} Player;

typedef struct GAME {
   int nMoves;      // total number of valid moves
   int status;      // 0: playing, 1: a winner, 2: a tie
   char stone;      // Current Player. 'W': for white stone, 'B' for black
   int x, y;        // (x, y): current move. x=col, y=row
   char board[8][8];// [x][y] AKA [col][row]
} Game;

typedef struct GAME_CONTEXT {
   int socketPlayer1;
   int socketPlayer2;
   int currentPlayerSocket;
   //Player *currentPlayer;
   Player *player1;
   Player *player2;
} GameContext;

List *scoreboard;

typedef struct INPUT_RESULT {
   int status;
   int row;
   int col;
} InputResult;

void setBoard(char board[8][8]);
void printBoard(char board[8][8]);
void placeStone(void *ptr1, void *ptr2);
int checkWin(void *gamePtr, void *gameContextPtr);
void *horizontalCheck(void *ptr);
void *verticalCheck(void *ptr);
void swapPlayer(void *gamePtr, void *gameContextPtr);
int verifyInput(int col, int row, void *gamePtr);
void *runInputThread(void *gamePtr);

