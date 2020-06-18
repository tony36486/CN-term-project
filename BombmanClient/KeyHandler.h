#pragma once
#ifndef KEY_HANDLER_H
#define KEY_HANDLER_H

#include <conio.h>
using namespace std;

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77
#define KEY_ENTER 13
#define KEY_SPACE 32
#define KEY_ESC 27
#define NO_KEY 0;

int getKeyPressed()
{
	unsigned char temp = _getch();
	if (temp == 224 || temp == 0) {
		temp = _getch();
		switch (temp) {
		case KEY_UP:
			return KEY_UP;
		case KEY_RIGHT:
			return KEY_RIGHT;
		case KEY_LEFT:
			return KEY_LEFT;
		case KEY_DOWN:
			return KEY_DOWN;
		}
	}

	switch (temp) {
	case KEY_ENTER:
		return KEY_ENTER;
	case KEY_SPACE:
		return KEY_SPACE;
	case KEY_ESC:
		return KEY_ESC;
	}
	
	return NO_KEY;
}

#endif