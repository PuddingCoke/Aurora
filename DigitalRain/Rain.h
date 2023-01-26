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

	float x;

	float y;

	float z;

	Timer time;

	static float stride;

	static constexpr float speedFactor = 400.f;

	static constexpr float maxRadius = 50.f;

	Rain()
	{
		re();
	}

	void re()
	{
		const float radius = Random::Float() * maxRadius;
		const float theta = Random::Float() * Math::two_pi;

		x = radius * cosf(theta);
		z = radius * sinf(theta);
		y = 30 + Random::Float() * 60;

		len = Random::Int() % 8 + 8;

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
		while (time.update(dt))
		{
			for (size_t i = len - 1ULL; i > 0; i--)
			{
				character[i] = character[i - 1ULL];
			}
			character[0] = Random::Int() % 94 + 33;
			y -= stride;
		}

		if (y + Rain::stride * len < -80)
		{
			re();
		}

	}

};

float Rain::stride;