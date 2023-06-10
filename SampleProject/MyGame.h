#pragma once

#include<Aurora/Game.h>
#include<Aurora/Resource/ComputeTexture.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComputeTexture* noiseTexture;

	MyGame() :
		noiseTexture(new ComputeTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8, FMT::RGBA8, FMT::RGBA8))
	{
	}

	~MyGame()
	{
		delete noiseTexture;
	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		RenderAPI::get()->GenNoise(noiseTexture->getUAVMip(0), Graphics::getWidth(), Graphics::getHeight());
		RenderAPI::get()->DebugDraw(noiseTexture);
	}


};