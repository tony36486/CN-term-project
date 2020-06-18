#pragma once
#ifndef INTERFACE_H
#define INTERFACE_H

#include <iostream>
using namespace std;

#define ITEM_COUNT 1
#define MENU_START_X 36	
#define MENU_START_Y 17

#define ITEM_WALL 2
#define ITEM_BOMB 1
#define ITEM_CLEAR 0

void gotoXY(int, int);
void clearSection(int, int, int, int);

void printChosen(int chosen)
{
	gotoXY(MENU_START_X, MENU_START_Y + (chosen % ITEM_COUNT));

	if (chosen)
		cout << "Enter";
	else
		clearSection(MENU_START_X, MENU_START_Y, MENU_START_X + 5, MENU_START_Y + ITEM_COUNT - 1);
}

void printTitle()
{
	cout << endl;
	cout << "      _|_|_|                              _|                                   " << endl;
	cout << "      _|    _|    _|_|    _|_|_|  _|_|    _|_|_|      _|_|    _|  _|_|         " << endl;
	cout << "      _|_|_|    _|    _|  _|    _|    _|  _|    _|  _|_|_|_|  _|_|             " << endl;
	cout << "      _|    _|  _|    _|  _|    _|    _|  _|    _|  _|        _|               " << endl;
	cout << "      _|_|_|      _|_|    _|    _|    _|  _|_|_|      _|_|_|  _|               " << endl;
	cout << endl;
	cout << "                                          _|      _|                           " << endl;
	cout << "                                          _|_|  _|_|    _|_|_|  _|_|_|         " << endl;
	cout << "                                          _|  _|  _|  _|    _|  _|    _|       " << endl;
	cout << "                                          _|      _|  _|    _|  _|    _|       " << endl;
	cout << "                                          _|      _|    _|_|_|  _|    _|       " << endl;
	cout << endl;
	cout << endl;
	cout << endl;
	cout << "                    ****************************************                   " << endl;
	cout << "                    *                                      *                   " << endl;
	cout << "                    *         Press Enter to start         *                   " << endl;
	cout << "                    *                                      *                   " << endl;
	cout << "                    ****************************************                   " << endl;
	cout << "                                                                               " << endl;
}

#define TABLE_OFFSET_X 30
#define TABLE_WIDTH 20
#define TABLE_HEIGHT 15

struct block
{
	unsigned int item;
	block* up;
	block* down;
	block* left;
	block* right;
};

void printInterface()
{
	for (int i = 0; i < TABLE_HEIGHT + 2; i++) {
		gotoXY(TABLE_OFFSET_X, i);
		for (int j = 0; j < TABLE_WIDTH + 2; j++) {
			if (i == 0 || i == TABLE_HEIGHT + 1 || j == 0 || j == TABLE_WIDTH + 1)
				cout << '\xDB';
			else
				cout << ' ';
		}
	}
}

void initInterface(block *map)
{
	//init pointer
	for (int i = 0; i < TABLE_HEIGHT; i++) {
		for (int j = 0; j < TABLE_WIDTH; j++) {
			map[i * TABLE_WIDTH + j].item = NULL;
			//up
			if (i == 0)
				map[i * TABLE_WIDTH + j].up = NULL;
			else
				map[i * TABLE_WIDTH + j].up = &map[(i - 1) * TABLE_WIDTH + j];
			//down
			if (i == TABLE_HEIGHT - 1)
				map[i * TABLE_WIDTH + j].down = NULL;
			else
				map[i * TABLE_WIDTH + j].down = &map[(i + 1) * TABLE_WIDTH + j];
			//left
			if (j == 0)
				map[i * TABLE_WIDTH + j].left = NULL;
			else
				map[i * TABLE_WIDTH + j].left = &map[i * TABLE_WIDTH + j - 1];
			//right
			if (j == TABLE_WIDTH + 1)
				map[i * TABLE_WIDTH + j].right = NULL;
			else
				map[i * TABLE_WIDTH + j].right = &map[i * TABLE_WIDTH + j + 1];
		}
	}
}

#endif