#pragma once

#include<Aurora/Game.h>
#include<Aurora/Texture2D.h>


//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame:public Game
{
public:

	Texture2D* texture;

	MyGame():
		texture(Texture2D::create("background.dds"))
	{
	}

	~MyGame()
	{
		delete texture;
	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		Graphics::clearDefRTV(DirectX::Colors::CadetBlue);
		Graphics::setDefRTV();
	}


};