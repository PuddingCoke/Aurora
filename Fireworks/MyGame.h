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
#include<Aurora/Timer.h>

#include"Objects.h"

class MyGame :public Game
{
public:

	PrimitiveBatch* pBatch;

	MyGame() :
		pBatch(PrimitiveBatch::create()),
		currentSkyColor{ 0.0f,0.0f,0.0f,1.0f },
		texture(RenderTexture::create(1920, 1080,DXGI_FORMAT_R8G8B8A8_UNORM))
	{

	}

	~MyGame()
	{
		delete pBatch;
		delete texture;
	}

	float sTime = 0;

	void update(const float& dt) override
	{
		sTime += dt;
	}

	void render() override
	{
		Graphics::setDefRTV();
		Graphics::clearDefRTV((float*)&Color::Black);
		pBatch->begin();
		pBatch->drawCircle(100, 100, 100, 1, 0, 0);
		pBatch->drawLine(100, 100, 100.f + 50.f * sinf(sTime / 12.f), 100.f + 50.f * cosf(sTime / 12.f), 0, 1, 0, 0.05f);
		pBatch->drawRoundCapLine(100, 100, 100 + 100 * sinf(sTime), 100 + 100 * cosf(sTime), 5, 0, 0, 1);
		pBatch->end();
	}

	const float GRAVITY = 0.9f;

	Color currentSkyColor;

	Color targetSkyColor;

	float speed;

	Timer timer;

	std::list<Star*> starActive;

	std::list<Star*> starPool;

	std::list<Spark*> sparkActive;

	std::list<Spark*> sparkPool;

	RenderTexture* texture;

};