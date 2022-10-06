#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A3D/FPSCamera.h>

#include"Ocean.h"

class MyGame :public Game
{
public:

	FPSCamera camera;

	Ocean ocean;

	Shader* debugShader;

	bool use=true;

	MyGame() :
		camera({ 0,0,0 }, { 1,0,0 }, { 0,1,0 }, 40, 3),
		ocean(1024, 2048, { 32.f,32.f }, 0.0005f),
		debugShader(Shader::fromFile("DebugShader.hlsl",ShaderType::Pixel))
	{
		camera.registerEvent();

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				std::cout << "C\n";
				use = !use;
			});
	}

	~MyGame()
	{
		delete debugShader;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		Renderer::clearDefRTV(DirectX::Colors::CadetBlue);
		Renderer::setDefRTV();

		Renderer::setBlendState(nullptr);
		Renderer::setViewport(1024, 1024);
		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Renderer::setSampler(0, States::linearClampSampler.Get());

		ocean.calcDisplacement();

		ocean.displacementY->setSRV(0);

		Shader::displayVShader->use();
		debugShader->use();

		Renderer::drawQuad();

		ID3D11ShaderResourceView* nullSRV = nullptr;

		Renderer::context->PSSetShaderResources(0, 1, &nullSRV);

	}


};