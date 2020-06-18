#include <Windows.h>
#include <iomanip>
#include <queue>
#include <ctime>
using namespace std;

#include "Bomb.h"
#include "Player.h"
#include "Interface.h"
#include "KeyHandler.h"
#include "ConsoleHandler.h"
#include "WinsockHandler.h"

bool initialize();
void startGame();
void playerUpdate();
void serverUpdate();
void graphicUpdate();

bool isOpen = true;

client packet{ -1, 0, STATE_LOBBY, -1, false };
Player player;

int main() {

	srand(time(NULL));

	bool success = false;
	success = initialize();

	if (success)
		startGame();

	if (success) {
		serverUpdate();//To retrieve final data
		graphicUpdate();//Generate final board
		closeConn(packet);
		string message;
		switch (player.getStatus()) {
		case STAT_DEAD:
			setColor(BG_RED);
			message = "You Died...";
			break;
		case STAT_ALIVE:
			setColor(BG_WHITE);
			message = "You Win...";
			break;
		case STAT_DISCONN:
			setColor(BG_GRAY);
			message = "You Have Been Disconnected...";
			break;
		}
		int temp = strlen(message.c_str());
		gotoXY(TABLE_OFFSET_X + TABLE_WIDTH / 2 - temp / 2, TABLE_HEIGHT / 2);
		cout << message;
		setColor(COLOR_DEFAULT);

		gotoXY(POS_STATUS);
		if (packet.state == STATE_GAMEOVER)
			cout << "Game Over\n";
		else
			cout << "Something Went wrong\n";
	}
	else {
		gotoXY(POS_STATUS);
		cout << "\nInitialization Failed\n";
	}
	system("PAUSE");
	return 0;
}

bool initialize()
{
	bool waiting = true;
	DWORD t;

	SetConsoleOutputCP(437);

	system("cls");
	printTitle();

	while (waiting) {
		t = GetTickCount();

		if (t % 1000 == 500)
			printChosen(1);
		else if (t % 1000 == 0)
			printChosen(0);

		if (_kbhit()) {
			if (getKeyPressed() == KEY_ENTER)
				waiting = false;
		}
	}

	//Connecting to server
	packet = con2serv(packet);
	//
	if (packet.confirm)
		return true;
	else
		return false;
}

//Game
#define START_POS_P1 0, 0
#define START_POS_P2 TABLE_WIDTH - 1, 0
#define START_POS_P3 0, TABLE_HEIGHT - 1
#define START_POS_P4 TABLE_WIDTH - 1, TABLE_HEIGHT - 1

#define DESIRED_FPS 10
#define DESIRED_TIME 1000 / DESIRED_FPS
#define MAX_FRAME_SKIP 5
#define PLAYER_NO packet.ID % 10

block Map[TABLE_WIDTH * TABLE_HEIGHT];
bool bombPlaced = false;
int bombLimit = 1;
int bombCount = 0;
queue<Bomb> updateQueue;

void startGame()
{
	DWORD proccessTime;
	bool playing = true;

	//Waiting for player
	while (packet.state == STATE_LOBBY) {
		int key = NO_KEY;
		if (_kbhit())
			key = getKeyPressed();
		if (key == KEY_ESC)
			return;

		packet.confirm = false;

		proccessTime = GetTickCount();
		//Only allow the player to change the state if there are more then 1 players
		if (packet.ready > 1) {
			if (key == KEY_ENTER)
				packet.state = STATE_READY;

			clearSection(SECT_STATUS);
			gotoXY(POS_STATUS);
			cout << "There are " << packet.ready << " players in the room, press Enter to start";
		}
		else {
			clearSection(SECT_STATUS);
			gotoXY(POS_STATUS);
			cout << "Waiting for other players...";
		}
		packet = updateServ(packet);
		proccessTime = GetTickCount() - proccessTime;

		if (!packet.confirm)
			return;

		if (proccessTime < DESIRED_TIME)
			Sleep(DESIRED_TIME - proccessTime);
	}
	//End waiting

	system("cls");

	//Board initialization
	initInterface(Map);
	if (packet.state == STATE_INIT) {
		packet = updateServ(packet);
		for (int i = 0; i < 2; i++) {
			for (int j = 0; j < 10; j++) {
				Map[packet.updateData[i].updateBomb[j].getY() * TABLE_WIDTH + packet.updateData[i].updateBomb[j].getX()].item = ITEM_WALL;
			}
		}
	}
	//End board initialization

	//Initialize Player
	gotoXY(0, 0);
	switch (PLAYER_NO) {
	case 0:
		player = *(new Player("P1", START_POS_P1));
		setColor(COLOR_WHITE);
		cout << "You are Player 1(White)";
		break;
	case 1:
		player = *(new Player("P2", START_POS_P2));
		setColor(COLOR_GREEN);
		cout << "You are Player 2(Green)";
		break;
	case 2:
		player = *(new Player("P3", START_POS_P3));
		setColor(COLOR_BLUE);
		cout << "You are Player 3(Blue)";
		break;
	case 3:
		player = *(new Player("P4", START_POS_P4));
		setColor(COLOR_YELLOW);
		cout << "You are Player 4(Yellow)";
		break;
	}
	setColor(COLOR_DEFAULT);
	//End initializing player

	packet.dataChanged = true;

	printInterface();

	//Game loop start
	DWORD timeStamp = GetTickCount();
	while (playing) {
		proccessTime = GetTickCount();

		if (player.getStatus() != STAT_DEAD && player.getStatus() != STAT_DISCONN) {
			playerUpdate();

			if (GetTickCount() - timeStamp > 10000) {
				if (rand() % 2) {
					if (bombLimit < 10)
						bombLimit++;
					else
						player.setPower(player.getPower() + 1);
				}

				else {
					if (player.getPower() < 10)
						player.setPower(player.getPower() + 1);
					else
						bombLimit++;
				}
				timeStamp = GetTickCount();
			}
			else {
				gotoXY(TABLE_OFFSET_X + TABLE_WIDTH + 2, TABLE_HEIGHT / 2 + 3);
				cout << "Next power-up in " << setw(2) << 10 - ((GetTickCount() - timeStamp) / 1000) << " seconds";
			}
		}
		else {
			string message;
			switch (player.getStatus()) {
			case STAT_DEAD:
				message = "You Died...";
				break;
			case STAT_DISCONN:
				message = "You Have Been Disconnected...";
				break;
			}
			int temp = strlen(message.c_str());
			setColor(BG_RED);
			gotoXY(TABLE_OFFSET_X + TABLE_WIDTH / 2 - temp / 2, TABLE_HEIGHT / 2);
			cout << message;
			setColor(COLOR_DEFAULT);
		}
		serverUpdate();
		if (packet.state == STATE_GAMEOVER)
			playing = false;

		if (!packet.confirm)
			return;
		if (player.getStatus() != STAT_DISCONN)
			graphicUpdate();

		//FPS Regulation Start
		proccessTime = GetTickCount() - proccessTime;
		if (proccessTime < DESIRED_TIME) {	
			clearSection(SECT_STATUS);
			Sleep(DESIRED_TIME - proccessTime);
		}
		else if (proccessTime > DESIRED_TIME) {
			int catchUpTime = proccessTime - DESIRED_TIME;
			int counter = 0;
			while (catchUpTime > 0 && counter++ < MAX_FRAME_SKIP) {
				clearSection(SECT_STATUS);
				gotoXY(POS_STATUS);
				cout << "Skipping Frames";

				proccessTime = GetTickCount();
				playerUpdate();
				proccessTime = GetTickCount() - proccessTime;
				catchUpTime -= DESIRED_TIME - proccessTime;
				if (counter > MAX_FRAME_SKIP) {
					clearSection(SECT_STATUS);
					gotoXY(POS_STATUS);
					cout << "Can't keep up";
					break;
				}
			}
		}//*///FPS Regulation End
	}
	//Game loop end
}

//Updates player's position, direction and bomb placing
void playerUpdate()
{
	int key;
	if (_kbhit()) {
		if (!bombPlaced) {
			Map[player.getY() * TABLE_WIDTH + player.getX()].item = ITEM_CLEAR;
		}

		key = getKeyPressed();
		switch (key) {
		case KEY_UP:
			player.setDir(DIR_UP);
			if (player.getY() - 1 >= 0 && Map[(player.getY() - 1) * TABLE_WIDTH + player.getX()].item == ITEM_CLEAR) {
				player.setY(player.getY() - 1);
				bombPlaced = false;
				packet.dataChanged = true;
			}
			break;
		case KEY_DOWN:
			player.setDir(DIR_DOWN);
			if (player.getY() + 1 < TABLE_HEIGHT && Map[(player.getY() + 1) * TABLE_WIDTH + player.getX()].item == ITEM_CLEAR) {
				player.setY(player.getY() + 1);
				bombPlaced = false;
				packet.dataChanged = true;
			}
			break;
		case KEY_LEFT:
			player.setDir(DIR_LEFT);
			if (player.getX() - 1 >= 0 && Map[player.getY() * TABLE_WIDTH + player.getX() - 1].item == ITEM_CLEAR) {
				player.setX(player.getX() - 1);
				bombPlaced = false;
				packet.dataChanged = true;
			}
			break;
		case KEY_RIGHT:
			player.setDir(DIR_RIGHT);
			if (player.getX() + 1 < TABLE_WIDTH && Map[player.getY() * TABLE_WIDTH + player.getX() + 1].item == ITEM_CLEAR) {
				player.setX(player.getX() + 1);
				bombPlaced = false;
				packet.dataChanged = true;
			}
			break;
		case KEY_SPACE:
			if (bombCount != bombLimit && !bombPlaced) {
				bombPlaced = true;
				FILETIME t;
				GetSystemTimeAsFileTime(&t);
				bombCount++;
				updateQueue.push(Bomb(packet.ID, player.getX(), player.getY(), t.dwLowDateTime, player.getPower()));
				packet.dataChanged = true;
				return;
			}
		}
	}
}

//Updates server and local data
void serverUpdate()
{

	packet.updateData[PLAYER_NO].dir = player.getDir();
	packet.updateData[PLAYER_NO].state = player.getStatus();
	packet.updateData[PLAYER_NO].posX = player.getX();
	packet.updateData[PLAYER_NO].posY = player.getY();
	packet.updateData[PLAYER_NO].bombCount = updateQueue.size();
	int i = 0;
	while (!updateQueue.empty())
	{
		packet.updateData[PLAYER_NO].updateBomb[i++] = updateQueue.front();
		updateQueue.pop();
	}

	packet.confirm = false;
	packet = updateServ(packet);
	packet.dataChanged = false;

	//TODO update game data

	for (int i = 0; i < packet.ready; i++) {
		for (int j = 0; j < packet.updateData[i].bombCount; j++) {
			if (packet.updateData[i].updateBomb[j].getState() == BOMB_STATE_NORMAL)
				Map[packet.updateData[i].updateBomb[j].getY() * TABLE_WIDTH + packet.updateData[i].updateBomb[j].getX()].item = ITEM_BOMB;
			else
				Map[packet.updateData[i].updateBomb[j].getY() * TABLE_WIDTH + packet.updateData[i].updateBomb[j].getX()].item = ITEM_CLEAR;

			if (packet.updateData[i].updateBomb[j].getState() == BOMB_STATE_EXPLODE) {
				if (i == PLAYER_NO)
					bombCount--;
			}
		}
	}
}

void bombExplode(int, int, int, int);

//Updates the graphic of the game
void graphicUpdate()
{
	clearSection(TABLE_OFFSET_X + 1, 1, TABLE_OFFSET_X + TABLE_WIDTH, TABLE_HEIGHT);
	//Print other players
	for (int i = 0; i < packet.ready; i++) {
		if (i == PLAYER_NO)
			continue;
		switch (i) {
		case 0:
			setColor(COLOR_WHITE);
			break;
		case 1:
			setColor(COLOR_GREEN);
			break;
		case 2:
			setColor(COLOR_BLUE);
			break;
		case 3:
			setColor(COLOR_YELLOW);
			break;
		}
		if (packet.updateData[i].dir != -1) {
			gotoXY(packet.updateData[i].posX + TABLE_OFFSET_X + 1, packet.updateData[i].posY + 1);
			if (packet.updateData[i].state == STAT_DEAD)
				cout << '\xF1';
			else
				cout << (char)packet.updateData[i].dir;
		}
	}
	//Print self on top of all others
	switch (PLAYER_NO) {
	case 0:
		setColor(COLOR_WHITE);
		break;
	case 1:
		setColor(COLOR_GREEN);
		break;
	case 2:
		setColor(COLOR_BLUE);
		break;
	case 3:
		setColor(COLOR_YELLOW);
		break;
	}
	gotoXY(packet.updateData[PLAYER_NO].posX + TABLE_OFFSET_X + 1, packet.updateData[PLAYER_NO].posY + 1);
	if (packet.updateData[PLAYER_NO].state == STAT_DEAD)
		cout << '\xF1';
	else
		cout << (char)packet.updateData[PLAYER_NO].dir;

	//Reset console color
	setColor(COLOR_DEFAULT);

	//Print player bomb count and power data
	gotoXY(TABLE_OFFSET_X + TABLE_WIDTH + 2, TABLE_HEIGHT / 2 + 1);
	cout << "Your bomb limit is " << bombLimit;
	gotoXY(TABLE_OFFSET_X + TABLE_WIDTH + 2, TABLE_HEIGHT / 2 + 2);
	cout << "Your bomb power is " << player.getPower();

	//Print bombs
	for (int i = 0; i < TABLE_WIDTH; i++) {
		for (int j = 0; j < TABLE_HEIGHT; j++) {
			if (Map[j * TABLE_WIDTH + i].item == ITEM_BOMB) {
				gotoXY(i + TABLE_OFFSET_X + 1, j + 1);
				cout << '\xA2';
			}
			else if (Map[j * TABLE_WIDTH + i].item == ITEM_WALL) {
				gotoXY(i + TABLE_OFFSET_X + 1, j + 1);
				cout << '\xDB';
			}
		}
	}

	//Generate explosion graph
	for (int i = 0; i < packet.ready; i++) {
		for (int j = 0; j < packet.updateData[i].bombCount; j++) {
			if (packet.updateData[i].updateBomb[j].getState() == BOMB_STATE_EXPLODE) {
				setColor(COLOR_RED);
				bombExplode(packet.updateData[i].updateBomb[j].getX(), packet.updateData[i].updateBomb[j].getY(), packet.updateData[i].updateBomb[j].getPower(), NULL);
				setColor(COLOR_DEFAULT);
			}
		}
	}
}

//A reccursive function that generates the bomb explosion graph, and also determins if the player is dead
void bombExplode(int posX, int posY, int power, int dir)
{
	if (posX == player.getX() && posY == player.getY()) {
		player.setStatus(STAT_DEAD);
		packet.dataChanged = true;
	}

	gotoXY(posX + TABLE_OFFSET_X + 1, posY + 1);
	if (power <= 3)
		cout << '\xB0';
	else if (power <= 7)
		cout << '\xB1';
	else
		cout << '\xB2';

	if (power == 0)
		return;

	else if (dir == NULL) {
		if (posY - 1 >= 0 && Map[posY * TABLE_WIDTH + posX].up != NULL && Map[posY * TABLE_WIDTH + posX].up->item != ITEM_WALL)
			bombExplode(posX, posY - 1, power - 1, DIR_UP);
		if (posY + 1 < TABLE_HEIGHT && Map[posY * TABLE_WIDTH + posX].down != NULL && Map[posY * TABLE_WIDTH + posX].down->item != ITEM_WALL)
			bombExplode(posX, posY + 1, power - 1, DIR_DOWN);
		if (posX - 1 >= 0 && Map[posY * TABLE_WIDTH + posX].left != NULL && Map[posY * TABLE_WIDTH + posX].left->item != ITEM_WALL)
			bombExplode(posX - 1, posY, power - 1, DIR_LEFT);
		if (posX + 1 < TABLE_WIDTH && Map[posY * TABLE_WIDTH + posX].right != NULL && Map[posY * TABLE_WIDTH + posX].right->item != ITEM_WALL)
			bombExplode(posX + 1, posY, power - 1, DIR_RIGHT);
	}

	else if (dir == DIR_UP) {
		if (Map[posY * TABLE_WIDTH + posX].up != NULL && Map[posY * TABLE_WIDTH + posX].up->item == ITEM_WALL)
			return;
		else if (posY - 1 >= 0)
			bombExplode(posX, posY - 1, power - 1, DIR_UP);
	}
	else if (dir == DIR_DOWN) {
		if (Map[posY * TABLE_WIDTH + posX].down != NULL && Map[posY * TABLE_WIDTH + posX].down->item == ITEM_WALL)
			return;
		else if (posY + 1 < TABLE_HEIGHT)
			bombExplode(posX, posY + 1, power - 1, DIR_DOWN);
	}
	else if (dir == DIR_LEFT) {
		if (Map[posY * TABLE_WIDTH + posX].left != NULL && Map[posY * TABLE_WIDTH + posX].left->item == ITEM_WALL)
			return;
		else if (posX - 1 >= 0)
			bombExplode(posX - 1, posY, power - 1, DIR_LEFT);
	}
	else if (dir == DIR_RIGHT) {
		if (Map[posY * TABLE_WIDTH + posX].right != NULL && Map[posY * TABLE_WIDTH + posX].right->item == ITEM_WALL)
			return;
		if (posX + 1 < TABLE_WIDTH)
			bombExplode(posX + 1, posY, power - 1, DIR_RIGHT);
	}
}