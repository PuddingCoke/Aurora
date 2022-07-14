#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/SpriteBatch.h>
#include<Aurora/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>

#include"Rain.h"

class MyGame :public Game
{
public:

	SpriteBatch* batch;

	BitmapFont* font;

	std::vector<Rain> rains;

	MyGame() :
		batch(SpriteBatch::create()),
		font(BitmapFont::create(L"Game_0.png", "Game.fnt", 24))
	{
		Rain::stride = font->getFontSize();

		for (size_t i = 0; i < Graphics::getWidth() / Rain::stride; i++)
		{
			rains.push_back(Rain((Graphics::getWidth() - Graphics::getWidth() / Rain::stride * Rain::stride) / 2 + i * Rain::stride, Random::Int() % 6 + 8));
		}
	}

	~MyGame()
	{
		delete batch;
		delete font;
	}

	void update(const float& dt) override
	{
		for (int i = 0; i < rains.size(); i++)
		{
			rains[i].update(dt);
			if (rains[i].y + Rain::stride * rains[i].len < 0)
			{
				rains[i].re();
			}
		}
	}

	void render() override
	{

		Graphics::setDefRTV();
		Graphics::clearDefRTV(DirectX::Colors::Black);
		
		Graphics::setBlendState(StateCommon::defBlendState.Get());

		batch->begin();
		for (int i = 0; i < rains.size(); i++)
		{
			batch->draw(font, rains[i].character[0], rains[i].x, rains[i].y, 1.f, 1.f, 1.f);
			for (int j = 1; j < rains[i].character.size(); j++)
			{
				batch->draw(font, rains[i].character[j], rains[i].x, rains[i].y + Rain::stride * j, 0, 1, 0, 1.f - (float)j / rains[i].character.size());
			}
		}
		batch->draw(font, "FPS:" + std::to_string(Graphics::getFPS()), 0, Graphics::getHeight(), 1, 1, 1, 1);
		batch->end();

	}
};