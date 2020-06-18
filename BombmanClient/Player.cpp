#include "Player.h"

Player::Player()
{
	_name = "anonymous";
	_posX = 0;
	_posY = 0;
	_dir = DIR_UP;
	_power = 1;
	_status = STAT_ALIVE;
}

Player::Player(string name)
{
	_name = name;
	_posX = 0;
	_posY = 0;
	_dir = DIR_UP;
	_power = 1;
	_status = STAT_ALIVE;
}

Player::Player(string name, int posX, int posY)
{
	_name = name;
	_posX = posX;
	_posY = posY;
	_dir = DIR_UP;
	_power = 1;
	_status = STAT_ALIVE;
}

Player::Player(string name, int posX, int posY, int power, int status)
{
	_name = name;
	_posX = posX;
	_posY = posY;
	_dir = DIR_UP;
	_power = power;
	_status = status;
}

string Player::getName()
{
	return _name;
}

int Player::getX()
{
	return _posX;
}

int Player::getY()
{
	return _posY;
}

char Player::getDir()
{
	return _dir;
}

int Player::getPower()
{
	return _power;
}

int Player::getStatus()
{
	return _status;
}

void Player::setName(string name)
{
	_name = name;
}

void Player::setX(int posX)
{
	_posX = posX;
}

void Player::setY(int posY)
{
	_posY = posY;
}

void Player::setDir(char dir)
{
	_dir = dir;
}

void Player::setPower(int power)
{
	_power = power;
}

void Player::setStatus(int status)
{
	_status = status;
}