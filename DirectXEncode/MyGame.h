#pragma once
#include<iostream>

#include<Aurora/Game.h>

#include<Aurora/Utils/Color.h>
#include<Aurora/EngineAPI/ImCtx.h>

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
		ImCtx::get()->OMSetDefRTV(nullptr);
		const Color color = { cosf(sTime),sinf(sTime),1.0 };
		ImCtx::get()->ClearDefRTV(color);
	}
};