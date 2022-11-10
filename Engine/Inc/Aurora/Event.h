#pragma once

#ifndef _EVENT_H_
#define _EVENT_H_

#include<map>
#include<functional>

class Event
{
public:

	Event();

	void operator-=(const int& id);

 	const int& operator+=(std::function<void(void)> func);

	void operator()();

private:

	int idx;

	std::map<int, std::function<void(void)>> functions;

};

#endif // !_EVENT_H_
