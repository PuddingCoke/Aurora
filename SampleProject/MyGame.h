#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Texture2D* texture;

	SpriteBatch* batch;

	MyGame() :
		texture(Texture2D::createRandNormal(64, 64)),
		batch(SpriteBatch::create())
	{

	}

	~MyGame()
	{

	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		Graphics::clearDefRTV(DirectX::Colors::CadetBlue);
		Graphics::setDefRTV();

		batch->begin();
		batch->draw(texture, 0, 0);
		batch->end();

	}


};