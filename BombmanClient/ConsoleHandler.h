#pragma once
#ifndef CONSOLE_HANDLER_H
#define CONSOLE_HANDLER_H

#include<Windows.h>
#include<iostream>

#define COLOR_DEFAULT 0x07
#define COLOR_GREEN 0x0A
#define COLOR_BLUE 0x0B
#define COLOR_RED 0x0C
#define COLOR_MAGENTA 0x0D
#define COLOR_YELLOW 0x0E
#define COLOR_WHITE 0x0F
#define BG_RED 0xC0
#define BG_WHITE 0xF0
#define BG_GRAY 0x80


void gotoXY(int xPos, int yPos)
{
	COORD scrn;
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	scrn.X = xPos; scrn.Y = yPos;
	SetConsoleCursorPosition(hOutput, scrn);
}

void setColor(int color)
{
	HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hOutput, color);
}

void clearSection(int xStart, int yStart, int xEnd, int yEnd)
{
	for (int i = yStart; i <= yEnd; i++) {
		gotoXY(xStart, i);
		for (int j = xStart; j <= xEnd; j++) {
			std::cout << " ";
		}
	}

}

#endif