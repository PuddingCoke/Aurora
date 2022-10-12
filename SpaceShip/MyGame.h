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
		spaceTexture(TextureCube::createEquirectangularMap("D:/SpaceShipAssets/space.hdr", 2048, { 0,1,0 })),
		skyboxPShader(Shader::fromFile("SkyboxPShader.hlsl", ShaderType::Pixel)),
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
		Renderer::clearDefRTV(DirectX::Colors::Black);
		Renderer::setDefRTV();

		Renderer::setBlendState(States::get()->defBlendState.Get());
		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Renderer::context->PSSetSamplers(0, 1, States::get()->linearClampSampler.GetAddressOf());

		TextureCube::shader->use();
		skyboxPShader->use();

		spaceTexture->setSRV(0);

		Renderer::draw(36, 0);
	}


};