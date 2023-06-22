#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/ResourceEssential.h>
#include<Aurora/Camera/OrbitCamera.h>
#include<Aurora/Effect/BloomEffect.h>

#include"TextBatch.h"
#include"Rain.h"

class MyGame :public Game
{
public:

	OrbitCamera camera;

	TextBatch* textBatch;

	RenderTexture* originTexture;

	DepthTexture* depthTexture;

	std::vector<Rain> rains;

	BloomEffect effect;

	static constexpr float bloomIntensity = 2.f;

	MyGame() :
		textBatch(new TextBatch("Game_0.png", "Game.fnt", 0.7f)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F)),
		depthTexture(new DepthTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::D32F)),
		camera({ 54,0,0 }, { 0,1,0 }, 20.f),
		effect(Graphics::getWidth(), Graphics::getHeight())
	{
		effect.setExposure(1.f);
		effect.setGamma(1.f);
		effect.setThreshold(0.f);
		effect.setIntensity(1.5f);
		effect.applyChange();

		camera.registerEvent();

		Rain::stride = textBatch->fontSize;

		for (size_t i = 0; i < 200; i++)
		{
			rains.push_back(Rain());
		}

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 0.1f, 512.f);
	}

	~MyGame()
	{
		delete textBatch;
		delete originTexture;
		delete depthTexture;
	}

	void update(const float& dt) override
	{
		//camera.rotateX(-dt * 0.8f);

		camera.applyInput(dt);

		for (int i = 0; i < rains.size(); i++)
		{
			rains[i].update(dt);
		}
	}

	void imGUICall() override
	{
		effect.imGUIEffectModifier();
	}

	void render() override
	{
		ImCtx::get()->ClearDSV(depthTexture, D3D11_CLEAR_DEPTH);
		ImCtx::get()->ClearRTV(originTexture->getMip(0), DirectX::Colors::Black);
		ImCtx::get()->OMSetRTV({ originTexture->getMip(0) }, depthTexture);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->RSSetState(States::rasterCullBack);
		ImCtx::get()->OMSetDepthStencilState(States::defDepthStencilState, 0);

		ImCtx::get()->OMSetBlendState(States::defBlendState);

		ImCtx::get()->RSSetState(States::rasterCullNone);

		for (int i = 0; i < rains.size(); i++)
		{
			textBatch->drawText(rains[i].character[0], rains[i].x, rains[i].y, rains[i].z, bloomIntensity, bloomIntensity, bloomIntensity, 1);
			for (int j = 1; j < rains[i].character.size(); j++)
			{
				textBatch->drawText(rains[i].character[j], rains[i].x, rains[i].y + Rain::stride * j, rains[i].z, 0.f, bloomIntensity * (1.f - (float)j / rains[i].character.size()), bloomIntensity * (1.f - (float)j / rains[i].character.size()), 1.0f);
			}
		}

		textBatch->render();

		ImCtx::get()->RSSetState(States::rasterCullBack);

		ShaderResourceView* bloomSRV = effect.process(originTexture);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(Shader::fullScreenPS);

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ImCtx::get()->PSSetSRV({ bloomSRV }, 0);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->DrawQuad();
	}
};