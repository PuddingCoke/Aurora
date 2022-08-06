#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>

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

	void update(const float& dt) override
	{

	}

	void render() override
	{
		Graphics::setDefRTV();
		Graphics::clearDefRTV(DirectX::Colors::Black);
		pBatch->begin();
		pBatch->drawCircle(100, 100, 100, 1, 0, 0);
		pBatch->drawLine(0, 0, 100, 100, 0, 1, 0);
		pBatch->drawRoundCapLine(100, 100, 200, 100, 5, 0, 0, 1);
		pBatch->end();
	}
};