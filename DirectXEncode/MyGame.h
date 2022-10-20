﻿#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/States.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Color.h>

class MyGame :public Game
{
public:

	MyGame()
	{

	}

	~MyGame()
	{
	}

	void update(const float& dt) override
	{
		sTime += dt;
	}

	float sTime = 0;

	void render() override
	{
		RenderAPI::get()->OMSetDefRTV(nullptr);
		const Color color = { cosf(sTime),sinf(sTime),1.0 };
		RenderAPI::get()->ClearDefRTV(color);
	}
};