#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/SpriteBatch.h>
#include<Aurora/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>

#include<WICTextureLoader.h>

class MyGame :public Game
{
public:

	Shader* pixelShader;

	Texture2D* noiseTexture;

	Texture2D* dustTexture;

	MyGame() :
		pixelShader(Shader::fromFile("GargantuaPShader.hlsl",ShaderType::Pixel)),
		noiseTexture(Texture2D::create(L"Noise.png")),
		dustTexture(Texture2D::create(L"Dust.png"))
	{

	}

	~MyGame()
	{
		delete pixelShader;
		delete noiseTexture;
		delete dustTexture;
	}

	void update(const float& dt) override
	{
		
	}

	void render() override
	{
		Graphics::setDefRTV();
		Graphics::clearDefRTV(DirectX::Colors::Black);

		Graphics::setViewport(Graphics::getWidth(), Graphics::getHeight());
		Graphics::setBlendState(StateCommon::addtiveBlend.Get());
		Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Graphics::context->PSSetSamplers(0, 1, StateCommon::defSamplerState.GetAddressOf());

		noiseTexture->setSRV(0);
		dustTexture->setSRV(1);

		Shader::displayVShader->use();
		pixelShader->use();

		Graphics::context->Draw(3, 0);

	}
};