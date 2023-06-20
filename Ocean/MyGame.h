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
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F)),
		depthTexture(new DepthTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::D32F, false)),
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

		ImCtx::get()->ClearRTV(originTexture->getMip(0), DirectX::Colors::Black);
		ImCtx::get()->OMSetRTV({ originTexture->getMip(0) }, nullptr);

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ImCtx::get()->PSSetSRV({ textureCube }, 0);
		ImCtx::get()->PSSetSampler({ States::linearWrapSampler }, 0);

		ImCtx::get()->BindShader(Shader::skyboxVS);
		ImCtx::get()->BindShader(skyboxPS);
		ImCtx::get()->HSUnbindShader();
		ImCtx::get()->GSUnbindShader();

		ImCtx::get()->DrawCube();

		ImCtx::get()->ClearDSV(depthTexture, D3D11_CLEAR_DEPTH);
		ImCtx::get()->OMSetRTV({ originTexture->getMip(0) }, depthTexture);

		ImCtx::get()->PSSetSRV({ textureCube,perlinTexture }, 1);

		ocean.render();

		ShaderResourceView* bloomSRV = effect.process(originTexture);

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);

		ImCtx::get()->PSSetSRV({ bloomSRV }, 0);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(Shader::fullScreenPS);

		ImCtx::get()->DrawQuad();
	}


};