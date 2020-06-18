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
	string _name;	//���a�ʺ�
	int _posX;		//���aX�y��
	int _posY;		//���aY�y��
	char _dir;		//���a��V
	int _power;		//���a�����Z��
	int _status;	//���a���A(����STAT_)
};

#endif