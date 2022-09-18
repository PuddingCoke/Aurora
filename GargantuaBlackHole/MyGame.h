#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A2D/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
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

	ComPtr<ID3D11SamplerState> samplerState;

	BloomEffect effect;

	bool bloom = true;

	float exposure;

	float gamma;

	MyGame() :
		pixelShader(Shader::fromFile("GargantuaPShader.hlsl", ShaderType::Pixel)),
		noiseTexture(Texture2D::createNoise(256, 256)),
		dustTexture(Texture2D::create("Dust.jpg")),
		accTexture(DoubleRTV::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		effect(Graphics::getWidth(), Graphics::getHeight())
	{
		//exposure = effect.getExposure();
		//gamma = effect.getGamma();

		exposure = 0.629999757f;
		gamma = 0.930000305f;

		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sd.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			Renderer::device->CreateSamplerState(&sd, samplerState.ReleaseAndGetAddressOf());
		}

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

		Renderer::setBlendState(StateCommon::addtiveBlend.Get());
		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Renderer::context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

		Shader::displayVShader->use();
		pixelShader->use();

		noiseTexture->setSRV(0);
		dustTexture->setSRV(1);
		accTexture->read()->getTexture()->setSRV(2);

		Renderer::context->Draw(3, 0);
		accTexture->swap();

		Renderer::context->PSSetShaderResources(0, 3, nullView);

		Renderer::clearDefRTV(DirectX::Colors::Black);

		if (bloom)
		{
			Texture2D* texture = effect.process(accTexture->read()->getTexture());

			Renderer::setDefRTV();

			Shader::displayPShader->use();

			texture->setSRV(0);

		}
		else
		{
			Renderer::setDefRTV();

			Shader::displayPShader->use();

			accTexture->read()->getTexture()->setSRV(0);
		}

		Renderer::context->Draw(3, 0);


		Renderer::context->PSSetShaderResources(0, 3, nullView);
	}
};