#include<Aurora/Event.h>

Event::Event():
	idx(0)
{
}

void Event::operator-=(const int& id)
{
	functions.erase(id);
}

const int& Event::operator+=(std::function<void(void)> func)
{
	functions.emplace(++idx, func);
	return idx;
}

void Event::operator()()
{
	for (auto& i : functions)
	{
		i.second();
	}
}