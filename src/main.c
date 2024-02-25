/*
  Copyright (C) 2024 Rares Hanganu

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <https://www.gnu.org/licenses/>.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_TABLE_SIZE 50
#define QSIZE 4096

// types of cells
#define EMPTY '-'
#define APPLE '*'
#define SNAKE '#'

// game controls
#define UP 'w'
#define RIGHT 'd'
#define DOWN 's'
#define LEFT 'a'

#define NDIR 4
#define MAXCH 128

typedef struct {
  int lin, col;
} Coordinate;

char table[MAX_TABLE_SIZE][MAX_TABLE_SIZE];

// line and colum differences for quickly changing
// the position of a cell
int dlin[NDIR] = {-1, 0, 1, 0};
int dcol[NDIR] = {0, 1, 0, -1};

// converts a key into a direction (0, 1, 2 or 3)
char key2dir[MAXCH];
char directions[NDIR] = {UP, RIGHT, DOWN, LEFT};

// the snake is held in a queue: each cell of the 
// snake is an element in the queue
int start, end; // the start and end of the queue
Coordinate snake[QSIZE];

void printTable(int score, int table_size);
void initTable(int table_size);
Coordinate generateApple(int table_size);
int moveSnake(int dir, int table_size, int *score);
int onTable(Coordinate coord, int table_size);

int main() {
  int running, table_size, key, dir, score;
  Coordinate temp;

  table_size = MAX_TABLE_SIZE + 1;
  while(table_size > MAX_TABLE_SIZE || table_size < 1) {
    printf("Enter table size: ");
    scanf("%d", &table_size);
  }

  initTable(table_size); // initialize the table
  
  for(dir = 0; dir < NDIR; dir++)
    key2dir[(int)directions[dir]] = dir;

  // initialize the snake
  start = 0;
  end = 1;
  snake[0].lin = snake[0].col = 0;
  table[0][0] = SNAKE;

  // initialize the apple
  temp = generateApple(table_size);
  table[temp.lin][temp.col] = APPLE;

  system("clear"); // clear the screen before starting

  running = score = 1;
  while(running) {
    // print information about the table and the score
    printTable(score, table_size);

    while(isspace(key = fgetc(stdin))); // read player input
    dir = key2dir[key]; // get direction code (0, 1, 2 or 3)
    running = moveSnake(dir, table_size, &score); // move the snake

    if(running) // only clear the screen if the game hasn't ended yet
      system("clear"); // clear the screen
  }

  printf("GAME OVER!\n");

  return 0;
}

// prints the table, as well as the current score
void printTable(int score, int table_size) {
  int l, c;

  printf("----------------------------\n");
  printf("- CURRENT SCORE : %d        -\n", score);
  printf("----------------------------\n\n");
  
  for(l = 0; l < table_size; l++) {
    for(c = 0; c < table_size; c++)
      fputc(table[l][c], stdout);
    fputc('\n', stdout);
  }
}

// initializes the table, should only be called at the start of the program
void initTable(int table_size) {
  int l, c;
  
  for(l = 0; l < table_size; l++)
    for(c = 0; c < table_size; c++)
      table[l][c] = EMPTY;
}

Coordinate generateApple(int table_size) {
  int l, c;

  while(table[(l = rand() % table_size)][(c = rand() % table_size)] != EMPTY);

  return (Coordinate){l, c};
}

int onTable(Coordinate coord, int table_size) {
  return coord.lin >= 0 && coord.lin < table_size &&
         coord.col >= 0 && coord.col < table_size;
}

// moves the snake, given the direction and the table size
// returns 1 if the game is still runing or 0 if the game is finished
int moveSnake(int dir, int table_size, int *score) {
  Coordinate temp;

  temp.lin = snake[(end - 1 + QSIZE) % QSIZE].lin + dlin[dir];
  temp.col = snake[(end - 1 + QSIZE) % QSIZE].col + dcol[dir];

  snake[end] = temp;
  end = (end + 1) % QSIZE;

  if(!onTable(temp, table_size) || (onTable(temp, table_size) && 
     table[temp.lin][temp.col] == SNAKE))
    return 0;

  // if we are on an apple, then the last cell of the
  // snake needs to be eliminated
  if(table[temp.lin][temp.col] != APPLE) {
    // update the table
    table[snake[start].lin][snake[start].col] = EMPTY;
    start = (start + 1) % QSIZE;
  } else {
    // we just ate an apple, so we need to generate a new one
    temp = generateApple(table_size);
    table[temp.lin][temp.col] = APPLE;
    (*score)++;
  }

  // update the table
  table[snake[(end - 1 + QSIZE) % QSIZE].lin]
       [snake[(end - 1 + QSIZE) % QSIZE].col] = SNAKE;
  
  return 1;
}