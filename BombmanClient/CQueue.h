#pragma once
#ifndef CQUEUE_H
#define CQUEUE_H

template <class Type>
class CQueue
{
public:
	CQueue(int);

	Type pop();
	void push(Type);
	void clear();

	int getSize();
	Type* getArray();

	bool isFull();
	bool isEmpty();

private:
	Type *_array;
	int _size;
	int _top;
	int _bottom;
};

#endif