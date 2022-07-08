#include<Aurora/Timer.h>

Timer::Timer() :
	curTime(0), timeLimit(0)
{

}

Timer::Timer(const float& timeLimit) :
	timeLimit(timeLimit), curTime(0)
{

}

bool Timer::update(const float& dt)
{
	if (curTime >= timeLimit)
	{
		curTime -= timeLimit;
		return true;
	}
	curTime += dt;
	return false;
}

void Timer::restart()
{
	curTime = 0;
}

void Timer::setTimeLimit(const float& timeLimit)
{
	this->timeLimit = timeLimit;
}

const float& Timer::getTimeLimit()
{
	return timeLimit;
}
