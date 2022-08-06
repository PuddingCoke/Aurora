#pragma once

#include<DirectXMath.h>
#include<vector>

class Cardioid
{
public:

	const float radius;

	const float offset;

	DirectX::XMFLOAT2 center;

	std::vector<DirectX::XMFLOAT2> points;

	Cardioid(const float& radius, const float& offset, const DirectX::XMFLOAT2& center, const size_t& pointNum) :
		radius(radius), offset(offset), center(center), points(pointNum)
	{
		for (size_t i = 0; i < pointNum; i++)
		{
			const float rads = 2.f * i / (float)pointNum * DirectX::XM_PI + offset;
			const float x = center.x + radius * cos(rads);
			const float y = center.y + radius * sin(rads);
			points[i] = DirectX::XMFLOAT2(x, y);
		}
	}

};