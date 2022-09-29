#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/States.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Color.h>
#include<Aurora/Timer.h>

#include"Cardioid.h"

class MyGame :public Game
{
public:

	PrimitiveBatch* pBatch;

	Cardioid c;

	size_t d1;

	static constexpr size_t d2 = 10000;

	Color color;

	float hue;

	int frameCount;

	static constexpr int frameLimit = 6;

	Timer timer;

	MyGame() :
		pBatch(PrimitiveBatch::create()),
		c(400, Math::pi / 2.f, DirectX::XMFLOAT2(Graphics::getWidth() / 2, Graphics::getHeight() / 2), 360),
		d1(0),
		hue(0),
		color{ 1,1,1 },
		frameCount(0),
		timer(0.004f)
	{
		pBatch->setLineWidth(2.f);
		pBatch->applyChange();
	}

	~MyGame()
	{
		delete pBatch;
	}

	void update(const float& dt) override
	{
		while (timer.update(dt))
		{
			d1 += 50ULL;
		}
	}

	void render() override
	{
		color = Color::HSVtoRGB({ (float)hue,1.f,1.f });
		Renderer::setBlendState(States::addtiveBlend.Get());
		Renderer::setDefRTV();
		Renderer::clearDefRTV(DirectX::Colors::Black);
		pBatch->begin();
		pBatch->drawCircle(c.center.x, c.center.y, c.radius, 1, 1, 1, 1);
		for (size_t i = 0; i < c.points.size(); i++)
		{
			pBatch->drawLine(c.points[i].x, c.points[i].y, c.points[(size_t)(i * d1 / d2) % c.points.size()].x, c.points[(size_t)(i * d1 / d2) % c.points.size()].y, color.r/2.f, color.g/2.f, color.b/2.f, 1);
			//pBatch->drawLine(c.points[i].x, c.points[i].y, c.points[(size_t)(i * d1 / d2) % c.points.size()].x, c.points[(size_t)(i * d1 / d2) % c.points.size()].y, 0.4, 0.4, 0.4, 1);
		}
		pBatch->end();
		if (frameCount++ == frameLimit)
		{
			hue += 1 / 360.f;
			frameCount = 0;
		}
	}
};