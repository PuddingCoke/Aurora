#pragma once

#include<Aurora/Game.h>

//这是一个模板项目，在项目选项中选择导出模板即可
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