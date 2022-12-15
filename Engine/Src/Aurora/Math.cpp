#include<Aurora/Math.h>

float Math::lerp(const float& x, const float& y, const float& s)
{
	return x * (1 - s) + y * s;
}

float Math::clamp(const float& value, const float& min, const float& max)
{
	if (value < min)
	{
		return min;
	}
	else if (value > max)
	{
		return max;
	}
	return value;
}

float Math::gauss(const float& sigma, const float& x)
{
	return 1.f / (sigma * 2.506628274631000502415765284811f) * exp(-0.5f * (x / sigma) * (x / sigma));
}
