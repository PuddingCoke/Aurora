#pragma once

#include<Aurora/Game.h>
#include<Aurora/Resource/ComputeTexture.h>
#include<Aurora/Resource/RenderComputeTexture.h>
#include<Aurora/Utils/Timer.h>

#include<Aurora/Effect/BloomEffect.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	class SwapComputeTexture
	{
	public:

		SwapComputeTexture(const UINT& width, const UINT& height) :
			cTexture1(new ComputeTexture(width, height, FMT::R8UI, FMT::R8UI, FMT::R8UI)),
			cTexture2(new ComputeTexture(width, height, FMT::R8UI, FMT::R8UI, FMT::R8UI))
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

	RenderComputeTexture* rcTexture;

	Shader* evolveCS;

	Shader* randomizeCS;

	Shader* visualizeCS;

	struct GameParam
	{
		DirectX::XMUINT2 mapSize;
		DirectX::XMFLOAT2 padding;
	} gameParam;

	ConstantBuffer* gameBuffer;

	Timer timer;

	BloomEffect bloomEffect;

	static constexpr UINT simulationHeight = 270;

	static constexpr UINT simulationWidth = simulationHeight * 16 / 9;

	MyGame() :
		timer(1.f / 60.f),
		swapTexture(new SwapComputeTexture(simulationWidth, simulationHeight)),
		rcTexture(new RenderComputeTexture(simulationWidth, simulationHeight, FMT::RGBA8UN, DirectX::Colors::Black)),
		evolveCS(new Shader("EvolveCS.hlsl", ShaderType::Compute)),
		randomizeCS(new Shader("RandomizeCS.hlsl", ShaderType::Compute)),
		visualizeCS(new Shader("VisualizeCS.hlsl", ShaderType::Compute)),
		bloomEffect(simulationWidth, simulationHeight)
	{
		gameParam.mapSize = DirectX::XMUINT2(simulationWidth, simulationHeight);

		gameBuffer = new ConstantBuffer(sizeof(GameParam), D3D11_USAGE_IMMUTABLE, &gameParam);

		randomize();

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				randomize();
			});

		bloomEffect.setThreshold(0.f);
		bloomEffect.applyChange();
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
		swapTexture->read()->clearUAV(value, 0);
		swapTexture->write()->clearUAV(value, 0);

		RenderAPI::get()->CSSetUAV({ swapTexture->write()->getUAVMip(0) }, 0);

		RenderAPI::get()->BindShader(randomizeCS);

		RenderAPI::get()->Dispatch(simulationWidth / 32, simulationHeight / 18, 1);
		swapTexture->swap();

		for (UINT i = 0; i < 60; i++)
		{
			step();
		}
	}

	void step()
	{
		RenderAPI::get()->CSSetUAV({ swapTexture->write()->getUAVMip(0) }, 0);
		RenderAPI::get()->CSSetSRV({ swapTexture->read() }, 0);

		RenderAPI::get()->CSSetConstantBuffer({ gameBuffer }, 1);

		RenderAPI::get()->BindShader(evolveCS);

		RenderAPI::get()->Dispatch(simulationWidth / 32, simulationHeight / 18, 1);
		swapTexture->swap();
	}

	void update(const float& dt) override
	{
		while (timer.update(dt))
		{
			step();
		}
	}

	void imGUICall() override
	{
		bloomEffect.imGUIEffectModifier();
	}

	void render()
	{
		RenderAPI::get()->CSSetUAV({ rcTexture->getUAVMip(0) }, 0);
		RenderAPI::get()->CSSetSRV({ swapTexture->read() }, 0);

		RenderAPI::get()->CSSetConstantBuffer({ gameBuffer }, 1);

		RenderAPI::get()->BindShader(visualizeCS);

		RenderAPI::get()->Dispatch(simulationWidth / 32, simulationHeight / 18, 1);

		ShaderResourceView* bloomSRV = bloomEffect.process(rcTexture);

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->PSSetSRV({ bloomSRV }, 0);
		RenderAPI::get()->PSSetSampler({ States::pointClampSampler }, 0);

		RenderAPI::get()->BindShader(RenderAPI::fullScreenVS);
		RenderAPI::get()->BindShader(RenderAPI::fullScreenPS);

		RenderAPI::get()->DrawQuad();
	}


};