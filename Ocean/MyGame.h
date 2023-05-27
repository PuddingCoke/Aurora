#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/ResourceEssential.h>
#include<Aurora/Camera/FPSCamera.h>
#include<Aurora/Effect/BloomEffect.h>

#include"Ocean.h"

class MyGame :public Game
{
public:

	FPSCamera camera;

	Ocean ocean;

	Shader* skyboxPS;

	RenderTexture* originTexture;

	DepthTexture* depthTexture;

	TextureCube* textureCube;

	ResourceTexture* perlinTexture;

	ComPtr<ID3D11InputLayout> inputLayout;

	BloomEffect effect;

	MyGame() :
		camera({ 1024,100,3584 }, { 0,-0.2f,-1.f }, { 0,1,0 }, 100),
		ocean(1024, 512, { 15.f,0.f }, 0.000003f),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		depthTexture(new DepthTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, false)),
		skyboxPS(new Shader("SkyboxPS.hlsl", ShaderType::Pixel)),
		textureCube(new TextureCube("ColdSunsetEquirect.png", 2048)),
		perlinTexture(new ResourceTexture("PerlinNoise.dds")),
		effect(Graphics::getWidth(), Graphics::getHeight())
	{
		camera.registerEvent();

		Camera::setProj(Math::pi / 5.f, Graphics::getAspectRatio(), 1.f, 4096.f);
	}

	~MyGame()
	{
		delete originTexture;
		delete depthTexture;
		delete skyboxPS;
		delete textureCube;
		delete perlinTexture;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void imGUICall() override
	{
		effect.imGUIEffectModifier();
	}

	void render()
	{
		ocean.update();

		originTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->PSSetSRV({ textureCube }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearWrapSampler }, 0);

		RenderAPI::skyboxVS->use();
		skyboxPS->use();
		RenderAPI::get()->HSUnbindShader();
		RenderAPI::get()->GSUnbindShader();

		RenderAPI::get()->DrawCube();

		depthTexture->clearDSV(D3D11_CLEAR_DEPTH);
		RenderAPI::get()->OMSetRTV({ originTexture }, depthTexture);

		RenderAPI::get()->PSSetSRV({ textureCube,perlinTexture }, 1);

		ocean.render();

		ShaderResourceView* bloomSRV = effect.process(originTexture);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->PSSetSRV({ bloomSRV }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
		
		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();
	}


};