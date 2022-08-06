#pragma once

#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/Graphics.h>
#include<Aurora/Timer.h>
#include<Aurora/Random.h>

#include<vector>

class Rain
{
public:

	std::vector<char> character;

	int len;

	int x, y;

	static float speedFactor;

	static int stride;

	//Speed = 165/timeLimit per sec

	Timer time;

	Rain(int _x, int _len) :time(_len / speedFactor), len(_len), character(_len)
	{
		x = _x;
		y = Graphics::getHeight() + Random::Int() % (Graphics::getHeight() / stride) * stride;
		for (size_t i = 0; i < _len; i++)
		{
			character[i] = Random::Int() % 94 + 33;
		}
	}

	void re()
	{
		y = Graphics::getHeight() + Random::Int() % (Graphics::getHeight() / stride) * stride;
		len = Random::Int() % 6 + 8;
		time.restart();
		time.setTimeLimit(len / speedFactor);
		character = std::vector<char>(len);
		for (size_t i = 0; i < len; i++)
		{
			character[i] = Random::Int() % 94 + 33;
		}
	}

	void update(const float& dt)
	{
		while (time.update(dt)) {
			for (size_t i = len - 1ULL; i > 0; i--)
			{
				character[i] = character[i - 1ULL];
			}
			character[0] = Random::Int() % 94 + 33;
			y -= stride;
		}
	}

};

float Rain::speedFactor = 300.f;
int Rain::stride;