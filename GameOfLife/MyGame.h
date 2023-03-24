#pragma once

#include<Aurora/Game.h>
#include<Aurora/ComputeTexture.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	class SwapComputeTexture
	{
	public:

		SwapComputeTexture(const UINT& width, const UINT& height) :
			cTexture1(new ComputeTexture(width, height, DXGI_FORMAT_R8_TYPELESS, DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8_UINT)),
			cTexture2(new ComputeTexture(width, height, DXGI_FORMAT_R8_TYPELESS, DXGI_FORMAT_R8_UNORM, DXGI_FORMAT_R8_UINT))
		{}

		~SwapComputeTexture()
		{
			delete cTexture1;
			delete cTexture2;
		}

		void swap()
		{
			ComputeTexture* temp = cTexture1;
			cTexture1 = cTexture2;
			cTexture2 = temp;
		}

		ShaderResourceView* read()
		{
			return cTexture1;
		}

		UnorderedAccessView* write()
		{
			return cTexture2;
		}

	private:

		ComputeTexture* cTexture1;

		ComputeTexture* cTexture2;

	} swapTexture;

	MyGame()
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
		RenderAPI::get()->GenNoise(noiseTexture, Graphics::getWidth(), Graphics::getHeight());
		RenderAPI::get()->DebugDraw(noiseTexture);
	}


};