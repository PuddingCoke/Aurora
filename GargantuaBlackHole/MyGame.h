#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/States.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/DoubleRTV.h>

#include<Aurora/PostProcessing/BloomEffect.h>

class MyGame :public Game
{
public:

	Shader* pixelShader;

	Texture2D* noiseTexture;

	Texture2D* dustTexture;

	DoubleRTV* accTexture;

	BloomEffect effect;

	bool bloom = true;

	float exposure;

	float gamma;

	MyGame() :
		pixelShader(Shader::fromFile("GargantuaPShader.hlsl", ShaderType::Pixel)),
		noiseTexture(new Texture2D(256, 256, Texture2D::TextureType::Noise)),
		dustTexture(new Texture2D("Dust.jpg")),
		accTexture(DoubleRTV::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		effect(Graphics::getWidth(), Graphics::getHeight())
	{
		//exposure = effect.getExposure();
		//gamma = effect.getGamma();

		exposure = 0.629999757f;
		gamma = 0.930000305f;

		accTexture->read()->clearRTV(DirectX::Colors::Black);
		accTexture->write()->clearRTV(DirectX::Colors::Black);

		Keyboard::addKeyDownEvent(Keyboard::K, [this]() {
			bloom = !bloom;
			});

		effect.setExposure(exposure);
		effect.setGamma(gamma);
		effect.setThreshold(0);
		effect.applyChange();
	}

	~MyGame()
	{
		delete pixelShader;
		delete noiseTexture;
		delete dustTexture;
		delete accTexture;
	}

	void update(const float& dt) override
	{
		if (Keyboard::getKeyDown(Keyboard::A))
		{
			exposure += 0.01f;
			effect.setExposure(exposure);
			effect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::S))
		{
			exposure -= 0.01f;
			effect.setExposure(exposure);
			effect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::H))
		{
			gamma += 0.01f;
			effect.setGamma(gamma);
			effect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::J))
		{
			gamma -= 0.01f;
			effect.setGamma(gamma);
			effect.applyChange();
		}
	}

	void render() override
	{
		ID3D11ShaderResourceView* nullView[3] = { nullptr,nullptr,nullptr };

		accTexture->write()->setRTV();
		accTexture->write()->clearRTV(DirectX::Colors::Black);

		Renderer::setBlendState(States::get()->addtiveBlend.Get());
		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Renderer::context->PSSetSamplers(0, 1, States::get()->linearWrapSampler.GetAddressOf());

		Shader::displayVShader->use();
		pixelShader->use();

		noiseTexture->PSSetSRV(0);
		dustTexture->PSSetSRV(1);
		accTexture->read()->PSSetSRV(2);

		Renderer::context->Draw(3, 0);
		accTexture->swap();

		Renderer::context->PSSetShaderResources(0, 3, nullView);

		Renderer::clearDefRTV(DirectX::Colors::Black);

		if (bloom)
		{
			Texture2D* texture = effect.process(accTexture->read());

			Renderer::setDefRTV();

			Shader::displayPShader->use();

			texture->PSSetSRV(0);

		}
		else
		{
			Renderer::setDefRTV();

			Shader::displayPShader->use();

			accTexture->read()->PSSetSRV(0);
		}

		Renderer::context->Draw(3, 0);


		Renderer::context->PSSetShaderResources(0, 3, nullView);
	}
};