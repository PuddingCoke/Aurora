#pragma once

#ifndef _MATH_H_
#define _MATH_H_

#include<DirectXMath.h>
#include<cmath>

namespace Math
{
	constexpr float half_pi = DirectX::XM_PIDIV2;
	constexpr float pi = DirectX::XM_PI;
	constexpr float two_pi = DirectX::XM_2PI;

	float lerp(const float& x, const float& y, const float& s);

	float clamp(const float& value, const float& min, const float& max);

}

#endif // !_MATH_H_