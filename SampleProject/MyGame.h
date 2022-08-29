#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<PerlinNoise/PerlinNoise.hpp>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Texture2D* texture;

	SpriteBatch* batch;

	MyGame() :
		texture(Texture2D::createNoise(512, 512)),
		batch(SpriteBatch::create())
	{
		std::cout << Random::Float() << "\n";
		std::cout << Random::Float() << "\n";
		std::cout << Random::Float() << "\n";
	}

	~MyGame()
	{
		delete batch;
		delete texture;
	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		Renderer::clearDefRTV(DirectX::Colors::CadetBlue);
		Renderer::setDefRTV();

		batch->begin();
		batch->draw(texture, 0, 0);
		batch->end();

	}


};