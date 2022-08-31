#pragma once

#include<Aurora/Math.h>
#include<Aurora/Graphics.h>
#include<Aurora/A2D/PrimitiveBatch.h>

class LogoDrawer
{
public:

	void setScale(const float& scale)
	{
		this->scale = scale;
	}

	void setPen(const bool& pen)
	{
		this->pen = pen;
	}

	void setPos(const float& xPos, const float& yPos)
	{
		x = xPos + Graphics::getWidth() / 2.f;
		y = yPos + Graphics::getHeight() / 2.f;
	}

	void rotate(const float& ang)
	{
		angle += ang;
	}

	void drawLine(const float& len)
	{
		PrimitiveBatch* const pBatch = PrimitiveBatch::get();

		const float destX = x + cosf(angle / 180.f * Math::pi) * len * scale;
		const float destY = y + sinf(angle / 180.f * Math::pi) * len * scale;

		if (pen)
		{
			pBatch->drawRoundCapLine(x, y, destX, destY, 3.f, 0, 0, 0);
		}

		x = destX;
		y = destY;

	}

private:

	float x = Graphics::getWidth() / 2.f;
	float y = Graphics::getHeight() / 2.f;
	float angle = 0;//deg
	float scale = 1.f;
	bool pen = true;


};