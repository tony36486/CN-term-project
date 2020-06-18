#include <Windows.h>
#include "Bomb.h"

Bomb::Bomb()
{
	_id = -1;
	_posX = 0;
	_posY = 0;
	_state = BOMB_STATE_NORMAL;
	_timer = 0;
	_power = 0;
}

Bomb::Bomb(int id, int x, int y, int power)
{
	_id = id;
	_posX = x;
	_posY = y;
	_state = BOMB_STATE_NORMAL;
	_timer = GetTickCount();
	_power = power;
}

Bomb::Bomb(int id, int x, int y, DWORD timer, int power)
{
	_id = id;
	_posX = x;
	_posY = y;
	_state = BOMB_STATE_NORMAL;
	_timer = timer;
	_power = power;
}

int Bomb::getId()
{
	return _id;
}

int Bomb::getX()
{
	return _posX;
}

int Bomb::getY()
{
	return _posY;
}

int Bomb::getState()
{
	return _state;
}

DWORD Bomb::getTimer()
{
	return _timer;
}

int Bomb::getPower()
{
	return _power;
}

void Bomb::setId(int id)
{
	_id = id;
}

void Bomb::setX(int x)
{
	_posX = x;
}

void Bomb::setY(int y)
{
	_posY = y;
}

void Bomb::setState(int state)
{
	_state = state;
}

void Bomb::setTimer(DWORD timer)
{
	_timer = timer;
}

void Bomb::setPower(int power)
{
	_power = power;
}