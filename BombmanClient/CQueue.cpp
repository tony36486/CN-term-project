#include "CQueue.h"

template <class Type>
CQueue<Type>::CQueue(int size)
{
	_array = new Type[size];

	_size = size;
	_top = 0;
	_bottom = 0;
}

template <class Type>
Type CQueue<Type>::pop()
{
	if (!isEmpty()) {
		top %= _size;
		return _array[top++];
	}
	else
		return NULL;
}

template <class Type>
void CQueue<Type>::push(Type item)
{
	if (!isFull()) {
		_array[_bottom] = item;
		_bottom = (++_bottom) % _size;
	}
}

template <class Type>
void CQueue<Type>::clear()
{
	while (!isEmpty()) {
		pop();
	}
}

template <class Type>
int CQueue<Type>::getSize()
{
	if (_top < _bottom)
		return _bottom - _top;
	else
		return _size - (_top - _bottom);
}

template <class Type>
Type* CQueue<Type>::getArray()
{
	return _array;
}

template <class Type>
bool CQueue<Type>::isFull()
{
	if (_top % _size == (_bottom + 1) % _size)
		return true;
	else
		return false;
}

template <class Type>
bool CQueue<Type>::isEmpty()
{
	if (_top % size == _bottom % _size)
		return true;
	else
		return false;
}