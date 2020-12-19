// Caleb bellisle
// Snake Game
// Fall 2020 CS 141

#include "SSDL.h"
#include <ctime>
#include <conio.h> 
#include <iostream>


using namespace std;
struct Location { int row_, clm_; };
								
								// Constants \\

enum			{ UP = 0, LEFT = 1, DOWN = 2, RIGHT = 3 };
enum	Items	{ WALL = 4, EMPTY = 5, TAIL = 6, HEAD = 7, FRUIT = 8 };
enum			{ ROWS = 25, CLMS = 25, VELO = 100, BOX = 20, BOXxy = 21 };

								//** Functions **\\
//initialize
void initialize(Items Board[ROWS][CLMS], int fRow, int fClm, Location Snake[], int& snakeLength);

//display
void display(Items Board[ROWS][CLMS], int fRow, int fClm, int tail, Location Snake[], int& snakeLength, int score);

// generate fruit x and y
int fruit();

// set/change direction
void changeDirection(char key, int tail, int& direction);

// move snake
void move(Items Board[ROWS][CLMS], int tail, Location Snake[], int& snakeLength, int direction);

// check for fruit
bool isFruit(Items Board[ROWS][CLMS], int fRow, int fClm, Location Snake[]);

// check for tail ouroboros (Tail eating snake)
bool ouroboros(Location Snake[], int MAX);

// check for wall
bool isWall(Location Snake[]);

// display end screen
void endScreen(int tail, int score);

								//** Main with game loop and variables **\\

int main(int argc, char** argv)
{
	SSDL_SetWindowSize(503, 503);
	srand(time(nullptr));

								//** variables **\\

	int direction = -1;									// Starts direction at null			

	int increaseScore = 3;								// how much the  tail is increased on each fruit
	int highScore = 0;


	int fruitRow = fruit();								// initial fruit x/y
	int fruitClm = fruit();

	bool playing = false;								// starts playing at false because you aren't playing
	bool restart = true;								// starts restart true because the game loops

	char key;

	int tail = 0;										// tail starts at 0
	
	int snakeLength = 1;


	Items Board[ROWS][CLMS];							// initialize game board

	constexpr int MAXSNAKE = 256;						// max snake
	Location Snake[MAXSNAKE];
	SSDL_SetFramesPerSecond(60);
														//** Game Loop **\\

	while (restart == true)
	{
		initialize(Board, fruitRow, fruitClm, Snake, snakeLength);						// initialize the board

		display(Board, fruitRow, fruitClm, tail, Snake, snakeLength, highScore);		// display the board

		SSDL_Image MENU = SSDL_LoadImage("media/menu.png");
		SSDL_RenderImage(MENU, -100, 100, 700, 400);									// Menu image
		SSDL_Music MENU_MUSIC = SSDL_LoadMUS("media/menu.ogg");

	
		SSDL_PlayMusic(MENU_MUSIC);
		SSDL_VolumeMusic(int(MIX_MAX_VOLUME * .05));

		SSDL_SetFramesPerSecond(60);

		SSDL_WaitKey(); 


		SSDL_PauseMusic();
		SSDL_Music PLAY_MUSIC = SSDL_LoadMUS("media/playing.ogg");


		SSDL_PlayMusic(PLAY_MUSIC);
		SSDL_Sound EAT = SSDL_LoadWAV("media/eat.mp3");

		playing = true;


		while (playing == true)
		{
			SSDL_DefaultEventHandler();

																						//*** DISPLAY THINGS ***
			SSDL_RenderClear();															//first, erase previous frame

			display(Board, fruitRow, fruitClm, tail, Snake, snakeLength, highScore);	// display board


																						// input section

			if (SSDL_IsKeyPressed('a'))													// take input
			{
				key = 'a';
				changeDirection(key, tail, direction);
			}

			if (SSDL_IsKeyPressed('w'))
			{
				key = 'w';
				changeDirection(key, tail, direction);
			}

			if (SSDL_IsKeyPressed('s'))
			{
				key = 's';
				changeDirection(key, tail, direction);
			}

			if (SSDL_IsKeyPressed('d'))
			{
				key = 'd';
				changeDirection(key, tail, direction);
			}


																				//*** UPDATE  THINGS ***

			move(Board, tail, Snake, snakeLength, direction);					// on input move

			if (isFruit(Board, fruitRow, fruitClm, Snake))						// check for fruit
			{
				SSDL_PlaySound(EAT);
				fruitRow = fruit();
				fruitClm = fruit();

				for (int i = 0; i < snakeLength; ++i)
				{
					if (fruitRow == Snake[i].row_ && fruitClm == Snake[i].clm_)	// if fruit is inside of snake, retry
					{
						fruitRow = fruit();
						fruitClm = fruit();
					}
				}

				snakeLength = snakeLength + increaseScore;						// increase score		
				tail++;

			}

			if (isWall(Snake))													// if wall, end game	
				playing = false;


			if (ouroboros(Snake, snakeLength))									// if snake ouroboros, (https://en.wikipedia.org/wiki/ouroboros) end game
				playing = false;

			if (highScore <= tail)												// if current game is greator than previous update score!
				highScore = tail;
		}


		SSDL_PausedMusic();

		SSDL_Music GAMEOVER = SSDL_LoadMUS("media/gameover.mp3");

		SSDL_PlayMusic(GAMEOVER);

		SSDL_Delay(500);

		endScreen(tail, highScore);

		tail = 0;															// reset tail to 0

		SSDL_RenderClear();
	}

																			// prevents the player from exiting too early.

	SSDL_WaitKey();
	return 0;
}

																			//** Functions **\\

void initialize(Items Board[ROWS][CLMS], int fRow, int fClm, Location Snake[], int& snakeLength)
{
	for (int i = 0; i < ROWS; ++i)
	{
		for (int j = 0; j < CLMS; ++j)
		{
			if (i == 0 || i == 23)
			{
				Board[i][j] = WALL;
			}
			if (j == 0 || j == 23)
			{
				Board[i][j] = WALL;
			}
			if (i != 0 && j != 0 && i != 23 && j != 23)
			{
				Board[i][j] = EMPTY;
			}
		}
	}
	for (int i = 1; i < 256; i++)
	{
		Snake[i] = { -1, -1 };
	}
	Snake[0] = { 1,1 };
	snakeLength = 1;
	Board[fRow][fClm] = FRUIT;
}

void display(Items Board[ROWS][CLMS], int fRow, int fClm, int tail, Location Snake[], int& snakeLength, int score)
{
	SSDL_SetCursor(250, 30);

	sout << "Score: " << tail;
	SSDL_SetCursor(250, 50);
	sout << "Highscore: " << score;

	for (int i = 0; i < 25; i++)
	{
		SSDL_RenderDrawRect(BOXxy * i, 0, BOX, BOX);		// top boxes
		SSDL_RenderDrawRect(0, BOXxy * i, BOX, BOX);		// left boxes
		SSDL_RenderDrawRect(BOXxy * i, 483, BOX, BOX);		// bottom boxes
		SSDL_RenderDrawRect(483, BOXxy * i, BOX, BOX);		// right boxes

	}

	for (int i = 0; i < 25; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			if (i == fRow && j == fClm)
			{
				SSDL_SetRenderDrawColor(RED);
				SSDL_RenderFillRect(fRow * BOXxy, fClm * BOXxy, BOX, BOX);		// draws the fruit
				SSDL_SetRenderDrawColor(WHITE);

			}
		}
	}

	//Draw that snake!
	SSDL_SetRenderDrawColor(GREEN);

	for (int i = 0; i < snakeLength; ++i)
		SSDL_RenderFillRect(Snake[i].row_ * BOXxy, Snake[i].clm_ * BOXxy, BOX, BOX);

	SSDL_SetRenderDrawColor(WHITE);
}

int fruit()
{
	int num = rand() % 23;

	while (num == 0)
	{
		num = rand() % 23;
	}
	return num;
}

void changeDirection(char key, int tail, int& direction)
{
	switch (key)
	{
	case 'w':
		if (direction != DOWN) direction = UP;
		else if (tail == 0)  direction = UP;
		break;

	case 'a':
		if (direction != RIGHT) direction = LEFT;
		else if (tail == 0)  direction = LEFT;
		break;

	case 's':
		if (direction != UP) direction = DOWN;
		else if (tail == 0)  direction = DOWN;
		break;

	case 'd':
		if (direction != LEFT) direction = RIGHT;
		else if (tail == 0)  direction = RIGHT;
		break;

	}

}

void move(Items Board[ROWS][CLMS], int tail, Location Snake[], int& snakeLength, int direction)
{
	SSDL_Delay(VELO);

	for (int i = snakeLength - 1; i >= 0; --i)
		Snake[i + 1] = Snake[i];

	switch (direction)
	{
	case UP: --Snake[0].clm_;
		break;

	case DOWN: ++Snake[0].clm_;
		break;

	case LEFT: --Snake[0].row_;
		break;

	case RIGHT: ++Snake[0].row_;
		break;

	}
}

bool isFruit(Items Board[ROWS][CLMS], int fRow, int fClm, Location Snake[])
{
	return Snake[0].row_ == fRow && Snake[0].clm_ == fClm;
}

bool isWall(Location Snake[])
{
	return Snake[0].row_ == 0 || Snake[0].clm_ == 0 || Snake[0].row_ == 23 || Snake[0].clm_ == 23;
}

bool ouroboros(Location Snake[], int MAX)
{
	for (int i = 1; i < MAX; i++)
	{
		if (Snake[0].row_ == Snake[i].row_)
		{
			if (Snake[0].clm_ == Snake[i].clm_)
			{
				return true;
			}
		}
	}
	return false;
}

void endScreen(int tail, int score)
{
	SSDL_Image END = SSDL_LoadImage("media/endGame.png");
	SSDL_RenderImage(END, -100, 100, 700, 400);						// Menu image

	SSDL_SetCursor(100, 300);
	sout << "Your score was: " << tail;
	SSDL_SetCursor(100, 320);
	sout << "Highscore: " << score;
	SSDL_SetCursor(100, 340);
	sout << "Press spacebar to restart the game!";

	SSDL_WaitKey();	
}