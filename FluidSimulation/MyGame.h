#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/SpriteBatch.h>
#include<Aurora/PrimitiveBatch.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Color.h>
#include<Aurora/DoubleRTV.h>

#include"Config.h"

class MyGame :public Game
{
public:

	//USAGE IMMUTABLE ConstantBufferB1
	ComPtr<ID3D11Buffer> constantb1;

	//USAGE DYNAMIC ConstantBufferB2
	ComPtr<ID3D11Buffer> constantb2;

	ComPtr<ID3D11SamplerState> linearSampler;

	ComPtr<ID3D11SamplerState> pointSampler;

	DoubleRTV* dye;//LINEAR
	DoubleRTV* velocity;//LINEAR
	DoubleRTV* pressure;//POINT

	RenderTexture* divergence;//POINT
	RenderTexture* curl;//POINT
	RenderTexture* sunrays;//LINEAR
	RenderTexture* sunraysTemp;//LINEAR

	MyGame()
	{
		{
			const DirectX::XMINT2 simRes = getResolution(config.SIM_RESOLUTION);
			const DirectX::XMINT2 dyeRes = getResolution(config.DYE_RESOLUTION);
			const DirectX::XMINT2 sunRes = getResolution(config.SUNRAYS_RESOLUTION);

			dye = new DoubleRTV(dyeRes.x, dyeRes.y, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
			
			velocity = new DoubleRTV(simRes.x, simRes.y, DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT);

			pressure = new DoubleRTV(simRes.x, simRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT);

			divergence = RenderTexture::create(simRes.x, simRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, DirectX::Colors::Transparent, false);

			curl = RenderTexture::create(simRes.x, simRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, DirectX::Colors::Transparent, false);

			sunrays = RenderTexture::create(sunRes.x, sunRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, DirectX::Colors::Transparent, false);

			sunraysTemp = RenderTexture::create(sunRes.x, sunRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, DirectX::Colors::Transparent, false);
			
			//创建数据不变的ConstantBuffer
			{
				struct ConstantBuffer
				{
					DirectX::XMFLOAT2 velocityTexelSize;
					DirectX::XMFLOAT2 screenTexelSize;
					DirectX::XMFLOAT2 sunraysTexelSizeX;
					DirectX::XMFLOAT2 sunraysTexelSizeY;
					float velocity_dissipation;
					float density_dissipation;
					float value;
					float aspectRatio;
					float curl;
					float radius;
					float weight;
					float v0;
				} constantBuffer{};

				constantBuffer.velocityTexelSize = DirectX::XMFLOAT2(velocity->renderTexture1->getTexture()->getTexelSizeX(), velocity->renderTexture1->getTexture()->getTexelSizeY());
				constantBuffer.screenTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());
				constantBuffer.sunraysTexelSizeX = DirectX::XMFLOAT2(sunrays->getTexture()->getTexelSizeX(), 0);
				constantBuffer.sunraysTexelSizeY = DirectX::XMFLOAT2(0, sunrays->getTexture()->getTexelSizeY());
				constantBuffer.velocity_dissipation = config.VELOCITY_DISSIPATION;
				constantBuffer.density_dissipation = config.DENSITY_DISSIPATION;
				constantBuffer.value = config.PRESSURE;
				constantBuffer.aspectRatio = Graphics::getAspectRatio();
				constantBuffer.curl = config.CURL;
				constantBuffer.radius = config.SPLAT_RADIUS / 100.f * Graphics::getAspectRatio();
				constantBuffer.weight = config.SUNRAYS_WEIGHT;

				D3D11_BUFFER_DESC cbd = {};
				cbd.ByteWidth = sizeof(ConstantBuffer);
				cbd.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
				cbd.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
				
				D3D11_SUBRESOURCE_DATA subResource = {};
				subResource.pSysMem = &constantBuffer;

				Graphics::device->CreateBuffer(&cbd, &subResource, constantb1.ReleaseAndGetAddressOf());
			}
		}

		//创建每帧要更新的ConstantBuffer
		{
			D3D11_BUFFER_DESC cbd = {};
			cbd.ByteWidth = sizeof(BufferDynamic);
			cbd.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			cbd.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;

			Graphics::device->CreateBuffer(&cbd, nullptr, constantb2.ReleaseAndGetAddressOf());
		}

		ID3D11Buffer* buffers[2] = { constantb1.Get(),constantb2.Get() };

		Graphics::context->VSSetConstantBuffers(1, 2, buffers);

		Graphics::context->PSSetConstantBuffers(1, 2, buffers);

	}

	~MyGame()
	{
		delete dye;
		delete velocity;
		delete pressure;
		delete divergence;
		delete curl;
		delete sunrays;
		delete sunraysTemp;
	}

	void update(const float& dt) override
	{

	}

	void render() override
	{
		Graphics::setDefRTV();
		Graphics::clearDefRTV(Color::Black);
	}
};