#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/SpriteBatch.h>
#include<Aurora/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Color.h>

#include<WICTextureLoader.h>

class MyGame :public Game
{
public:

	PrimitiveBatch* pBatch;

	MyGame() :
		pBatch(PrimitiveBatch::create())
	{

	}

	~MyGame()
	{
		delete pBatch;
	}

	float sTime = 0;

	void update(const float& dt) override
	{
		sTime += dt;
	}

	void render() override
	{
		Graphics::setDefRTV();
		Graphics::clearDefRTV((float*)&Color::Gold);
		pBatch->begin();
		pBatch->drawCircle(100, 100, 100, 1, 0, 0);
		pBatch->drawLine(100, 100, 100.f + 50.f * sinf(sTime / 12.f), 100.f + 50.f * cosf(sTime / 12.f), 0, 1, 0);
		pBatch->drawRoundCapLine(100, 100, 100 + 100 * sinf(sTime), 100 + 100 * cosf(sTime), 5, 0, 0, 1);
		pBatch->end();
	}
};