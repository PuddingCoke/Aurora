#pragma once

#include<Aurora/Game.h>
#include<Aurora/A3D/TextureCube.h>
#include<Aurora/A3D/FPSCamera.h>

class MyGame :public Game
{
public:

	TextureCube* spaceTexture;

	Shader* skyboxPShader;

	FPSCamera camera;

	MyGame() :
		spaceTexture(TextureCube::createEquirectangularMap("D:/SpaceShipAssets/space.hdr", 2048)),
		skyboxPShader(new Shader("SkyboxPShader.hlsl", ShaderType::Pixel)),
		camera({ 0,0,0 }, { 1,0,0 }, { 0,1,0 }, 100.f, 3.f)
	{
		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);
		
		camera.registerEvent();
	}

	~MyGame()
	{
		delete spaceTexture;
		delete skyboxPShader;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->OMSetBlendState(States::get()->defBlendState.Get());
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 1, 1);

		TextureCube::skyboxVS->use();
		skyboxPShader->use();

		RenderAPI::get()->PSSetSRV({ spaceTexture }, 0);

		RenderAPI::get()->DrawCube();
	}


};