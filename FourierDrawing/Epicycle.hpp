#pragma once

#include<math.h>

class Epicycle
{
public:

	double re;
	double im;

	double length;
	double freq;
	double theta;

	Epicycle() :
		length(0), freq(0), theta(0), re(0), im(0) {}

	Epicycle(const double& length, const double& freq, const double& theta) :
		length(length), freq(freq), theta(theta), re(0), im(0) {}

	void set(const double& t)
	{
		re = length * cos(freq * t + theta);
		im = length * sin(freq * t + theta);
	}

};
