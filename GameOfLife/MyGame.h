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
		bloomEffect(ImCtx::get(), simulationWidth, simulationHeight)
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
		ImCtx::get()->ClearUAV(swapTexture->read()->getMip(0), value);
		ImCtx::get()->ClearUAV(swapTexture->write()->getMip(0), value);

		ImCtx::get()->CSSetUAV({ swapTexture->write()->getMip(0) }, 0);

		ImCtx::get()->BindShader(randomizeCS);

		ImCtx::get()->Dispatch(simulationWidth / 32, simulationHeight / 18, 1);
		swapTexture->swap();

		for (UINT i = 0; i < 60; i++)
		{
			step();
		}
	}

	void step()
	{
		ImCtx::get()->CSSetUAV({ swapTexture->write()->getMip(0) }, 0);
		ImCtx::get()->CSSetSRV({ swapTexture->read() }, 0);

		ImCtx::get()->CSSetConstantBuffer({ gameBuffer }, 1);

		ImCtx::get()->BindShader(evolveCS);

		ImCtx::get()->Dispatch(simulationWidth / 32, simulationHeight / 18, 1);
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
		ImCtx::get()->CSSetUAV({ rcTexture->getMip(0) }, 0);
		ImCtx::get()->CSSetSRV({ swapTexture->read() }, 0);

		ImCtx::get()->CSSetConstantBuffer({ gameBuffer }, 1);

		ImCtx::get()->BindShader(visualizeCS);

		ImCtx::get()->Dispatch(simulationWidth / 32, simulationHeight / 18, 1);

		ShaderResourceView* bloomSRV = bloomEffect.process(rcTexture);

		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);

		ImCtx::get()->PSSetSRV({ bloomSRV }, 0);
		ImCtx::get()->PSSetSampler({ States::pointClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(Shader::fullScreenPS);

		ImCtx::get()->DrawQuad();
	}


};