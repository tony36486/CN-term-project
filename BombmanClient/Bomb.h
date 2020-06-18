#pragma once
#ifndef BOMB_H
#define BOMB_H

#define BOMB_STATE_NORMAL 0
#define BOMB_STATE_EXPLODE 1
#define BOMB_STATE_MOVING 2

class Bomb
{
public:
	Bomb();
	Bomb(int, int, int, int);
	Bomb(int, int, int, DWORD, int);

	int getId();
	int getX();
	int getY();
	int getState();
	DWORD getTimer();
	int getPower();

	void setId(int);
	void setX(int);
	void setY(int);
	void setState(int);
	void setTimer(DWORD);
	void setPower(int);

private:
	int _id;
	int _posX;
	int _posY;
	int _state;
	DWORD _timer;
	int _power;
};

#endif