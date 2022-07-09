#pragma once

#ifndef _COLOR_H_
#define _COLOR_H_

#include"Random.h"

struct Color
{
	float r;
	float g;
	float b;
	float a = 1.f;

	int toInt() const;

	bool operator==(const Color& color);

	static Color random();

	static Color HSVtoRGB(const Color& c);

	static const Color Gold;

	static const Color White;

	static const Color Invisible;

	static const Color Black;

};

#endif // !_COLOR_H_
