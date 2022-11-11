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

	ResourceTexture* noiseTexture;

	ResourceTexture* dustTexture;

	DoubleRTV* accTexture;

	BloomEffect effect;

	bool bloom = true;

	float exposure;

	float gamma;

	MyGame() :
		pixelShader(new Shader("GargantuaPShader.hlsl", ShaderType::Pixel)),
		noiseTexture(new ResourceTexture(256, 256, Texture2D::TextureType::Noise)),
		dustTexture(new ResourceTexture("Dust.jpg")),
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
		RenderAPI::get()->OMSetRTV({ accTexture->write() }, nullptr);
		accTexture->write()->clearRTV(DirectX::Colors::Black);

		RenderAPI::get()->OMSetBlendState(States::addtiveBlend);
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->PSSetSampler({ States::linearWrapSampler }, 0);

		RenderAPI::fullScreenVS->use();
		pixelShader->use();

		RenderAPI::get()->PSSetSRV({ noiseTexture,dustTexture,accTexture->read() }, 0);
		RenderAPI::get()->Draw(3, 0);

		accTexture->swap();

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);

		if (bloom)
		{
			ShaderResourceView* textureSRV = effect.process(accTexture->read());

			RenderAPI::get()->OMSetDefRTV(nullptr);

			RenderAPI::fullScreenPS->use();

			RenderAPI::get()->PSSetSRV({ textureSRV }, 0);
		}
		else
		{
			RenderAPI::get()->OMSetDefRTV(nullptr);

			RenderAPI::fullScreenPS->use();

			RenderAPI::get()->PSSetSRV({ accTexture->read() }, 0);
		}

		RenderAPI::get()->Draw(3, 0);
	}
};