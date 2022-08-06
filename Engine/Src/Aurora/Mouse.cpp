#include<Aurora/Mouse.h>

float Mouse::x = 0;
float Mouse::y = 0;

float Mouse::dx = 0;
float Mouse::dy = 0;

Event Mouse::moveEvent;
Event Mouse::leftDownEvent;
Event Mouse::rightDownEvent;
Event Mouse::leftUpEvent;
Event Mouse::rightUpEvent;

bool Mouse::leftDown = false;
bool Mouse::rightDown = false;

const float& Mouse::getX()
{
	return x;
}

const float& Mouse::getY()
{
	return y;
}

const float& Mouse::getDX()
{
	return dx;
}

const float& Mouse::getDY()
{
	return dy;
}

const bool& Mouse::getLeftDown()
{
	return leftDown;
}

const bool& Mouse::getRightDown()
{
	return rightDown;
}

int Mouse::addMoveEvent(std::function<void(void)> func)
{
	return moveEvent += func;
}

int Mouse::addLeftDownEvent(std::function<void(void)> func)
{
	return leftDownEvent += func;
}

int Mouse::addRightDownEvent(std::function<void(void)> func)
{
	return rightDownEvent += func;
}

int Mouse::addLeftUpEvent(std::function<void(void)> func)
{
	return leftUpEvent += func;
}

int Mouse::addRightUpEvent(std::function<void(void)> func)
{
	return rightUpEvent += func;
}

void Mouse::removeMoveEvent(const int& id)
{
	moveEvent -= id;
}

void Mouse::removeLeftDownEvent(const int& id)
{
	leftDownEvent -= id;
}

void Mouse::removeRightDownEvent(const int& id)
{
	rightDownEvent -= id;
}

void Mouse::removeLeftUpEvent(const int& id)
{
	leftUpEvent -= id;
}

void Mouse::removeRightUpEvent(const int& id)
{
	rightUpEvent -= id;
}
