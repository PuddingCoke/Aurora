#pragma once

#include<Aurora/Color.h>

struct Pointer
{
	int id = -1;
	float texcoordX = 0;
	float texcoordY = 0;
	float prevTexcoordX = 0;
	float prevTexcoordY = 0;
	float deltaX = 0;
	float deltaY = 0;
	bool down = false;
	bool moved = false;
	float r = 30;
	float g = 0;
	float b = 300;

	void makeColorRandom()
	{
		const Color c = Color::HSVtoRGB({ Random::Float(),1.f,1.f });
		r = c.r * 0.15f;
		g = c.g * 0.15f;
		b = c.b * 0.15f;
	}

} pointer;