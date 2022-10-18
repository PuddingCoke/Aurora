#pragma once
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

	BloomEffect bloomEffect;

	OrbitCamera camera;

	bool rotating = true;

	float exposure;

	float gamma;

	MyGame() :
		attractor(1000000),
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		depthStencilView(DepthStencilView::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT)),
		camera({ 4,4,-11 }, { -1,-1,-1 })
	{
		camera.registerEvent();

		gamma = bloomEffect.getGamma();
		exposure = bloomEffect.getExposure();

		bloomEffect.setThreshold(0);

		bloomEffect.applyChange();

		Keyboard::addKeyDownEvent(Keyboard::K, [this]() {
			rotating = !rotating;
			});
	}

	~MyGame()
	{
		delete renderTexture;
		delete depthStencilView;
	}

	void update(const float& dt) override
	{
		if (rotating)
		{
			camera.rotateX(dt);
		}

		if (Keyboard::getKeyDown(Keyboard::A))
		{
			exposure += 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::S))
		{
			exposure -= 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::H))
		{
			gamma += 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::J))
		{
			gamma -= 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
		}

		attractor.update(dt);
	}

	void render() override
	{
		depthStencilView->clear(D3D11_CLEAR_DEPTH);
		renderTexture->clearRTV(DirectX::Colors::Transparent);
		RenderAPI::get()->OMSetRTV({ renderTexture }, depthStencilView->get());

		attractor.render();

		ShaderResourceView* const textureSRV = bloomEffect.process(renderTexture);
		
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);
		RenderAPI::get()->PSSetSRV({ textureSRV }, 0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		RenderAPI::get()->DrawQuad();
	}
};