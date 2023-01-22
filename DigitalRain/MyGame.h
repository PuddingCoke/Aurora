﻿#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/A3D/OrbitCamera.h>
#include<Aurora/PostProcessing/BloomEffect.h>

#include"TextBatch.h"
#include"Rain.h"

class MyGame :public Game
{
public:

	OrbitCamera camera;

	TextBatch* textBatch;

	RenderTexture* originTexture;

	DepthStencilView* depthView;

	std::vector<Rain> rains;

	BloomEffect effect;

	static constexpr float bloomIntensity = 2.f;

	MyGame() :
		textBatch(new TextBatch("Game_0.png", "Game.fnt", 1)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		depthView(new DepthStencilView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT)),
		camera({ 50,20,0 }, { 0,1,0 }, 20.f),
		effect(Graphics::getWidth(), Graphics::getHeight())
	{
		effect.setExposure(1.f);
		effect.setGamma(1.f);
		effect.setThreshold(0.f);
		effect.setIntensity(1.5f);
		effect.applyChange();

		camera.registerEvent();

		Rain::stride = textBatch->fontSize;

		for (size_t i = 0; i < 150; i++)
		{
			rains.push_back(Rain());
		}

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 0.1f, 200.f);
	}

	~MyGame()
	{
		delete textBatch;
		delete originTexture;
		delete depthView;
	}

	void update(const float& dt) override
	{
		camera.rotateX(-dt);

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
		depthView->clear(D3D11_CLEAR_DEPTH);
		originTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ originTexture }, depthView);

		RenderAPI::get()->OMSetBlendState(States::defBlendState);

		RenderAPI::get()->RSSetState(States::rasterCullNone);

		for (int i = 0; i < rains.size(); i++)
		{
			textBatch->drawText(rains[i].character[0], rains[i].x, rains[i].y, rains[i].z, bloomIntensity, bloomIntensity, bloomIntensity, 1);
			for (int j = 1; j < rains[i].character.size(); j++)
			{
				textBatch->drawText(rains[i].character[j], rains[i].x, rains[i].y + Rain::stride * j, rains[i].z, 0.f, bloomIntensity * (1.f - (float)j / rains[i].character.size()), 0.f, 1.0f);
			}
		}

		textBatch->render();

		RenderAPI::get()->RSSetState(States::rasterCullBack);

		ShaderResourceView* bloomSRV = effect.process(originTexture);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->PSSetSRV({ bloomSRV }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::get()->DrawQuad();
	}
};