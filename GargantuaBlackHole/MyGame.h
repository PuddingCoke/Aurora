#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/SpriteBatch.h>
#include<Aurora/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/DoubleRTV.h>

#include<Aurora/PostProcessing/BloomEffect.h>

class MyGame :public Game
{
public:

	Shader* pixelShader;

	Shader* displayShader;

	Shader* displayShader2;

	Texture2D* noiseTexture;

	Texture2D* dustTexture;

	DoubleRTV* accTexture;

	ComPtr<ID3D11SamplerState> samplerState;

	BloomEffect effect;

	RenderTexture* renderTexture;

	bool bloom = true;

	MyGame() :
		pixelShader(Shader::fromFile("GargantuaPShader.hlsl", ShaderType::Pixel)),
		displayShader(Shader::fromFile("DisplayPShader.hlsl", ShaderType::Pixel)),
		displayShader2(Shader::fromFile("DisplayPShader2.hlsl",ShaderType::Pixel)),
		noiseTexture(Texture2D::create(L"Noise.png")),
		dustTexture(Texture2D::create(L"Dust.jpg")),
		accTexture(new DoubleRTV(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		effect(Graphics::getWidth(), Graphics::getHeight()),
		renderTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black, false))
	{
		{
			D3D11_SAMPLER_DESC sd = {};
			sd.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sd.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sd.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sd.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			sd.MinLOD = 0;
			sd.MaxLOD = D3D11_FLOAT32_MAX;

			Graphics::device->CreateSamplerState(&sd, samplerState.ReleaseAndGetAddressOf());
		}

		accTexture->read()->clearRTV(DirectX::Colors::Black);
		accTexture->write()->clearRTV(DirectX::Colors::Black);

		Keyboard::addKeyDownEvent(Keyboard::K, [this]() {
			bloom = !bloom;
			});
	}

	~MyGame()
	{
		delete pixelShader;
		delete displayShader;
		delete displayShader2;
		delete noiseTexture;
		delete dustTexture;
		delete accTexture;
		delete renderTexture;
	}

	void update(const float& dt) override
	{

	}

	void render() override
	{
		ID3D11RenderTargetView* nullRTV = nullptr;
		ID3D11ShaderResourceView* nullView[3] = { nullptr,nullptr,nullptr };

		accTexture->write()->setRTV();
		accTexture->write()->clearRTV(DirectX::Colors::Black);

		Graphics::setBlendState(StateCommon::addtiveBlend.Get());
		Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Graphics::context->PSSetSamplers(0, 1, samplerState.GetAddressOf());

		Shader::displayVShader->use();
		pixelShader->use();

		noiseTexture->setSRV(0);
		dustTexture->setSRV(1);
		accTexture->read()->getTexture()->setSRV(2);

		Graphics::context->Draw(3, 0);
		accTexture->swap();

		Graphics::context->OMSetRenderTargets(1, &nullRTV, nullptr);
		Graphics::context->PSSetShaderResources(0, 3, nullView);

		if (bloom)
		{
			renderTexture->setRTV();
			renderTexture->clearRTV(DirectX::Colors::Black);

			displayShader->use();

			accTexture->read()->getTexture()->setSRV(0);

			Graphics::context->Draw(3, 0);

			Graphics::context->OMSetRenderTargets(1, &nullRTV, nullptr);
			Graphics::context->PSSetShaderResources(0, 3, nullView);

			Texture2D* texture = effect.process(renderTexture->getTexture());

			Graphics::setDefRTV();
			Graphics::clearDefRTV(DirectX::Colors::Black);

			Shader::displayPShader->use();

			texture->setSRV(0);

		}
		else
		{
			Graphics::setDefRTV();
			Graphics::clearDefRTV(DirectX::Colors::Black);

			displayShader2->use();

			accTexture->read()->getTexture()->setSRV(0);
		}

		Graphics::context->Draw(3, 0);

		Graphics::context->OMSetRenderTargets(1, &nullRTV, nullptr);
		Graphics::context->PSSetShaderResources(0, 3, nullView);

	}
};