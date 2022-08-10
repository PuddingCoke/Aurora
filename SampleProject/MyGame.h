#pragma once

#include<Aurora/Game.h>

class MyGame:public Game
{
public:


	void update(const float& dt) override
	{

	}

	void render()
	{
		Graphics::clearDefRTV(DirectX::Colors::CadetBlue);
		Graphics::setDefRTV();
	}


};