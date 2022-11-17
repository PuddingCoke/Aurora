#pragma once

#include<Aurora/Game.h>
#include<Aurora/A3D/TextureCube.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/ComputeTexture.h>

class MyGame :public Game
{
public:

	TextureCube* spaceTexture;

	Shader* skyboxPShader;

	FPSCamera camera;

	ComputeTexture* noiseTexture;

	MyGame() :
		spaceTexture(new TextureCube("D:/Assets/SpaceShip/space.hdr", 2048)),
		skyboxPShader(new Shader("SkyboxPShader.hlsl", ShaderType::Pixel)),
		camera({ 0,0,0 }, { 1,0,0 }, { 0,1,0 }, 100.f),
		noiseTexture(new ComputeTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM))
	{
		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);

		camera.registerEvent();
	}

	~MyGame()
	{
		delete spaceTexture;
		delete skyboxPShader;
		delete noiseTexture;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->OMSetBlendState(States::defBlendState);
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::skyboxVS->use();
		skyboxPShader->use();

		RenderAPI::get()->PSSetSRV({ spaceTexture }, 0);

		RenderAPI::get()->DrawCube();
	}


};