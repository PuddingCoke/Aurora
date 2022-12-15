﻿#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/Event.h>
#include<Aurora/States.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/DX/View/DepthStencilView.h>
#include<Aurora/A3D/OrbitCamera.h>

#include"StrangeAttractor.h"

#include<Aurora/PostProcessing/BloomEffect.h>

//关于这种吸引子
//https://gereshes.com/2020/01/13/attracted-to-attractors/
class MyGame :public Game
{
public:

	StrangeAttractor attractor;

	DepthStencilView* depthStencilView;

	RenderTexture* renderTexture;

	Buffer* simulationBuffer;

	BloomEffect bloomEffect;

	OrbitCamera camera;

	bool rotating = true;

	struct SimulationParam
	{
		float factor;
		DirectX::XMFLOAT3 reserved;
	} param;

	MyGame() :
		attractor(2000000),
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		depthStencilView(new DepthStencilView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT)),
		camera({ 4,4,-11 }, { -1,-1,-1 }, 2.f),
		simulationBuffer(new Buffer(sizeof(SimulationParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, 0, D3D11_CPU_ACCESS_WRITE))
	{
		camera.registerEvent();

		bloomEffect.setThreshold(0);
		bloomEffect.applyChange();

		Keyboard::addKeyDownEvent(Keyboard::K, [this]() {
			rotating = !rotating;
			});

		param.factor = 0.100f;

		memcpy(simulationBuffer->map(0).pData, &param, sizeof(SimulationParam));
		simulationBuffer->unmap(0);

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 100.f);
	}

	~MyGame()
	{
		delete renderTexture;
		delete depthStencilView;
		delete simulationBuffer;
	}

	void imGUICall() override
	{
		bloomEffect.imGUIBloomEffectModifier();
		ImGui::SliderFloat("Dissipative Factor", &param.factor, 0.f, 0.2f);
	}

	void update(const float& dt) override
	{
		param.factor = 0.100 + 0.01f * sinf(Graphics::getSTime());

		memcpy(simulationBuffer->map(0).pData, &param, sizeof(SimulationParam));
		simulationBuffer->unmap(0);

		if (rotating)
		{
			camera.applyInput(dt);
			camera.rotateX(dt);
		}

		RenderAPI::get()->CSSetConstantBuffer({ simulationBuffer }, 1);

		attractor.update(dt);
	}

	void render() override
	{
		depthStencilView->clear(D3D11_CLEAR_DEPTH);
		renderTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ renderTexture }, depthStencilView);

		attractor.render();

		ShaderResourceView* const textureSRV = bloomEffect.process(renderTexture);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
		RenderAPI::get()->PSSetSRV({ textureSRV }, 0);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();
	}
};