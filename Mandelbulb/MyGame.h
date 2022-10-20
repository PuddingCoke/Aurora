#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<PerlinNoise/PerlinNoise.hpp>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ResourceTexture* texture;

	SpriteBatch* batch;

	MyGame() :
		texture(new ResourceTexture(1024, 1024, Texture2D::TextureType::Gauss)),
		batch(SpriteBatch::create())
	{
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
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::CadetBlue);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		batch->begin();
		batch->draw(texture, texture, 0, 0);
		batch->end();
	}


};