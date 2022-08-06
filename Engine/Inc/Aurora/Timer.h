#pragma once

#ifndef _TIMER_H_
#define _TIMER_H_

class Timer
{
public:

	Timer();

	Timer(const float& timeLimit);

	bool update(const float& dt);

	void restart();

	void setTimeLimit(const float& timeLimit);

	const float& getTimeLimit() const;

private:

	float timeLimit;

	float curTime;

};


#endif // !_TIMER_H_
