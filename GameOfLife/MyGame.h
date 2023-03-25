#pragma once

#include<Aurora/Game.h>
#include<Aurora/ComputeTexture.h>
#include<Aurora/RCTexture.h>
#include<Aurora/Timer.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	class SwapComputeTexture
	{
	public:

		SwapComputeTexture(const UINT& width, const UINT& height) :
			cTexture1(new ComputeTexture(width, height, DXGI_FORMAT_R8_UINT)),
			cTexture2(new ComputeTexture(width, height, DXGI_FORMAT_R8_UINT))
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

		ComputeTexture* read()
		{
			return cTexture1;
		}

		ComputeTexture* write()
		{
			return cTexture2;
		}

	private:

		ComputeTexture* cTexture1;

		ComputeTexture* cTexture2;

	} *swapTexture;

	RCTexture* rcTexture;

	Shader* evolveCS;

	Shader* randomizeCS;

	Shader* visualizeCS;

	struct GameParam
	{
		DirectX::XMUINT2 mapSize;
		DirectX::XMFLOAT2 padding;
	} gameParam;

	Buffer* gameBuffer;

	Timer timer;

	static constexpr UINT simulationHeight = 270;

	static constexpr UINT simulationWidth = simulationHeight * 16 / 9;

	MyGame() :
		timer(0.01f),
		swapTexture(new SwapComputeTexture(simulationWidth, simulationHeight)),
		rcTexture(new RCTexture(simulationWidth, simulationHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black)),
		evolveCS(new Shader("EvolveCS.hlsl", ShaderType::Compute)),
		randomizeCS(new Shader("RandomizeCS.hlsl", ShaderType::Compute)),
		visualizeCS(new Shader("VisualizeCS.hlsl", ShaderType::Compute))
	{
		randomize();

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				randomize();
			});

		gameParam.mapSize = DirectX::XMUINT2(simulationWidth, simulationHeight);

		gameBuffer = new Buffer(sizeof(GameParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &gameParam);
	}

	~MyGame()
	{
		delete swapTexture;
		delete rcTexture;
		delete evolveCS;
		delete randomizeCS;
		delete visualizeCS;
		delete gameBuffer;
	}

	void randomize()
	{
		const UINT value[] = { 0,0,0,0 };
		swapTexture->read()->clear(value);
		swapTexture->write()->clear(value);

		RenderAPI::get()->CSSetUAV({ swapTexture->write() }, 0);

		randomizeCS->use();

		RenderAPI::get()->Dispatch(swapTexture->write()->getWidth() / 32, swapTexture->write()->getHeight() / 18, 1);
		swapTexture->swap();
	}

	void update(const float& dt) override
	{
		while (timer.update(dt))
		{
			RenderAPI::get()->CSSetUAV({ swapTexture->write() }, 0);
			RenderAPI::get()->CSSetSRV({ swapTexture->read() }, 0);

			RenderAPI::get()->CSSetConstantBuffer({ gameBuffer }, 1);

			evolveCS->use();

			RenderAPI::get()->Dispatch(swapTexture->write()->getWidth() / 32, swapTexture->write()->getHeight() / 18, 1);
			swapTexture->swap();
		}
	}

	void render()
	{
		RenderAPI::get()->CSSetUAV({ rcTexture }, 0);
		RenderAPI::get()->CSSetSRV({ swapTexture->read() }, 0);

		RenderAPI::get()->CSSetConstantBuffer({ gameBuffer }, 1);

		visualizeCS->use();

		RenderAPI::get()->Dispatch(rcTexture->getWidth() / 32, rcTexture->getHeight() / 18, 1);

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->PSSetSRV({ rcTexture }, 0);
		RenderAPI::get()->PSSetSampler({ States::pointClampSampler }, 0);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();
	}


};