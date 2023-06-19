#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Input/Mouse.h>
#include<Aurora/Input/Event.h>
#include<Aurora/EngineAPI/States.h>
#include<Aurora/Resource/RenderTexture.h>
#include<Aurora/Resource/DepthTexture.h>
#include<Aurora/Camera/OrbitCamera.h>

#include"StrangeAttractor.h"

#include<Aurora/Effect/BloomEffect.h>

//关于这种吸引子
//https://gereshes.com/2020/01/13/attracted-to-attractors/
class MyGame :public Game
{
public:

	StrangeAttractor attractor;

	DepthTexture* depthTexture;

	RenderTexture* renderTexture;

	ConstantBuffer* simulationBuffer;

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
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F, DirectX::Colors::Black)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		depthTexture(new DepthTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::D32F)),
		camera({ 4,4,-11 }, { -1,-1,-1 }, 2.f),
		simulationBuffer(new ConstantBuffer(sizeof(SimulationParam), D3D11_USAGE_DYNAMIC))
	{
		camera.registerEvent();

		bloomEffect.setExposure(0.6f);
		bloomEffect.setThreshold(1.0f);
		bloomEffect.applyChange();

		Keyboard::addKeyDownEvent(Keyboard::K, [this]() {
			rotating = !rotating;
			});

		param.factor = 0.180f;

		memcpy(simulationBuffer->map().pData, &param, sizeof(SimulationParam));
		simulationBuffer->unmap();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 0.01f, 100.f);
	}

	~MyGame()
	{
		delete renderTexture;
		delete depthTexture;
		delete simulationBuffer;
	}

	void imGUICall() override
	{
		bloomEffect.imGUIEffectModifier();
		ImGui::SliderFloat("Dissipative Factor", &param.factor, 0.f, 0.2f);
	}

	void update(const float& dt) override
	{
		//param.factor = 0.1f + 0.01f * sinf(Graphics::getSTime());

		memcpy(simulationBuffer->map().pData, &param, sizeof(SimulationParam));
		simulationBuffer->unmap();

		if (rotating)
		{
			camera.applyInput(dt);
			camera.rotateX(dt);
		}
	}

	void render() override
	{
		RenderAPI::get()->CSSetConstantBuffer({ simulationBuffer }, 1);

		attractor.update(Graphics::getDeltaTime());

		RenderAPI::get()->ClearDSV(depthTexture, D3D11_CLEAR_DEPTH);
		RenderAPI::get()->ClearRTV(renderTexture->getMip(0), DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ renderTexture->getMip(0) }, depthTexture);

		attractor.render();

		ShaderResourceView* const textureSRV = bloomEffect.process(renderTexture);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
		RenderAPI::get()->PSSetSRV({ textureSRV }, 0);

		RenderAPI::get()->BindShader(RenderAPI::fullScreenVS);
		RenderAPI::get()->BindShader(RenderAPI::fullScreenPS);

		RenderAPI::get()->DrawQuad();
	}
};