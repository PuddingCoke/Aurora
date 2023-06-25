#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/ResourceEssential.h>
#include<Aurora/Utils/Timer.h>

#include<Aurora/Effect/BloomEffect.h>
#include<Aurora/Effect/FXAAEffect.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	SwapTexture<ComputeTexture>* swapTexture;

	ComputeTexture* outputTexture;

	Shader* randomizeCS;

	Shader* evolveCS;

	Shader* visualizeCS;

	Timer timer;

	BloomEffect bloomEffect;

	FXAAEffect fxaaEffect;

	static constexpr UINT simulationHeight = 1080;

	static constexpr UINT simulationWidth = simulationHeight * 16 / 9;

	MyGame() :
		timer(1.f / 60.f),
		swapTexture(new SwapTexture<ComputeTexture>([=] {return new ComputeTexture(simulationWidth, simulationHeight, FMT::R16F, FMT::R16F, FMT::R16F); })),
		outputTexture(new ComputeTexture(simulationWidth, simulationHeight, FMT::RGBA8UN, FMT::RGBA8UN, FMT::RGBA8UN)),
		randomizeCS(new Shader(Utils::getRootFolder() + "RandomizeCS.cso", ShaderType::Compute)),
		evolveCS(new Shader(Utils::getRootFolder() + "EvolveCS.cso", ShaderType::Compute)),
		visualizeCS(new Shader(Utils::getRootFolder() + "VisualizeCS.cso", ShaderType::Compute)),
		bloomEffect(ImCtx::get(), simulationWidth, simulationHeight),
		fxaaEffect(ImCtx::get(), simulationWidth, simulationHeight)
	{
		randomize();

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				randomize();
			});

		bloomEffect.setThreshold(0.f);
		bloomEffect.setIntensity(0.6f);
		bloomEffect.applyChange();
	}

	~MyGame()
	{
		delete swapTexture;
		delete randomizeCS;
		delete evolveCS;
		delete visualizeCS;
		delete outputTexture;
	}

	void randomize()
	{
		const float value[] = { 0,0,0,0 };
		ImCtx::get()->ClearUAV(swapTexture->read()->getMip(0), value);
		ImCtx::get()->ClearUAV(swapTexture->write()->getMip(0), value);

		ImCtx::get()->CSSetUAV({ swapTexture->write()->getMip(0) }, 0);

		ImCtx::get()->BindShader(randomizeCS);

		ImCtx::get()->Dispatch(simulationWidth / 16, simulationHeight / 9, 1);
		swapTexture->swap();
	}

	void evolve()
	{
		ImCtx::get()->CSSetUAV({ swapTexture->write()->getMip(0) }, 0);
		ImCtx::get()->CSSetSRV({ swapTexture->read() }, 0);
		ImCtx::get()->CSSetSampler({ States::pointWrapSampler }, 0);

		ImCtx::get()->BindShader(evolveCS);

		ImCtx::get()->Dispatch(simulationWidth / 16, simulationHeight / 9, 1);
		swapTexture->swap();
	}

	void update(const float& dt) override
	{
		while (timer.update(dt))
		{
			evolve();
		}
	}

	void imGUICall() override
	{
		bloomEffect.imGUIEffectModifier();
	}

	void render()
	{
		ImCtx::get()->CSSetUAV({ outputTexture->getMip(0) }, 0);
		ImCtx::get()->CSSetSRV({ swapTexture->read() }, 0);

		ImCtx::get()->BindShader(visualizeCS);

		ImCtx::get()->Dispatch(simulationWidth / 16, simulationHeight / 9, 1);

		ShaderResourceView* bloomSRV = bloomEffect.process(outputTexture);

		ShaderResourceView* antialiasedSRV = fxaaEffect.process(bloomSRV);

		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);

		ImCtx::get()->PSSetSRV({ antialiasedSRV }, 0);
		ImCtx::get()->PSSetSampler({ States::pointClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(Shader::fullScreenPS);

		ImCtx::get()->DrawQuad();
	}


};