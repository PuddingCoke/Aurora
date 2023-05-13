#pragma once

#include<Aurora/Game.h>
#include<Aurora/Core/ResourceEssential.h>
#include<Aurora/Core/Camera/FPSCamera.h>

#include"Ocean.h"

class MyGame :public Game
{
public:

	FPSCamera camera;

	Ocean ocean;

	Shader* skyboxPS;

	DepthTexture* depthTexture;

	TextureCube* textureCube;

	ResourceTexture* perlinTexture;

	ResourceTexture* foamBubbleTexture;

	ResourceTexture* foamIntensityTexture;

	ResourceTexture* windGustsTexture;

	ComPtr<ID3D11InputLayout> inputLayout;

	MyGame() :
		camera({ 1024,100,3584 }, { 0,-0.2f,-1.f }, { 0,1,0 }, 100),
		ocean(1024, 512, { 20.f,0.f }, 0.000003f),
		depthTexture(new DepthTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, true)),
		skyboxPS(new Shader("SkyboxPS.hlsl", ShaderType::Pixel)),
		textureCube(new TextureCube("ColdSunsetEquirect.png",2048)),
		perlinTexture(new ResourceTexture("PerlinNoise.dds")),
		foamBubbleTexture(new ResourceTexture("foam_bubbles.dds")),
		foamIntensityTexture(new ResourceTexture("foam_intensity.dds")),
		windGustsTexture(new ResourceTexture("wind_gusts.dds"))
	{
		camera.registerEvent();

		Camera::setProj(Math::pi / 5.f, Graphics::getAspectRatio(), 1.f, 4096.f);
	}

	~MyGame()
	{
		delete depthTexture;
		delete skyboxPS;
		delete textureCube;
		delete perlinTexture;
		delete foamBubbleTexture;
		delete foamIntensityTexture;
		delete windGustsTexture;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		ocean.update();

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::AliceBlue);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->PSSetSRV({ textureCube }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearWrapSampler }, 0);

		RenderAPI::skyboxVS->use();
		skyboxPS->use();
		RenderAPI::get()->HSUnbindShader();
		RenderAPI::get()->GSUnbindShader();

		RenderAPI::get()->DrawCube();

		depthTexture->clearDSV(D3D11_CLEAR_DEPTH);
		RenderAPI::get()->OMSetDefRTV(depthTexture);

		RenderAPI::get()->PSSetSRV({ textureCube,perlinTexture,foamBubbleTexture,foamIntensityTexture,windGustsTexture }, 1);

		ocean.render();
	}


};