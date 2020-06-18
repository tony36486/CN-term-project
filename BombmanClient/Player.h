#pragma once
#ifndef PLAYER_H
#define PLAYER_H

#include<string>
using namespace std;

#define STAT_DEAD 0
#define STAT_ALIVE 1
#define STAT_INVINCIBLE 2
#define STAT_DISCONN 3


#define DIR_UP 0x1E
#define DIR_RIGHT 0x10
#define DIR_LEFT 0x11
#define DIR_DOWN 0x1F

class Player
{
public:
	Player();
	Player(string);
	Player(string, int, int);
	Player(string, int, int, int, int);

	string getName();
	int getX();
	int getY();
	char getDir();
	int getPower();
	int getStatus();

	void setName(string);
	void setX(int);
	void setY(int);
	void setDir(char);
	void setPower(int);
	void setStatus(int);

private:
	string _name;	//玩家暱稱
	int _posX;		//玩家X座標
	int _posY;		//玩家Y座標
	char _dir;		//玩家方向
	int _power;		//玩家攻擊距離
	int _status;	//玩家狀態(應用STAT_)
};

#endif