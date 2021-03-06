#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/Event.h>
#include<Aurora/StateCommon.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Color.h>
#include<Aurora/DoubleRTV.h>

#include"Config.h"
#include"Pointer.h"

class MyGame :public Game
{
public:

	//USAGE IMMUTABLE ConstantBufferB1
	ComPtr<ID3D11Buffer> constantb1;

	//USAGE DYNAMIC ConstantBufferB2
	ComPtr<ID3D11Buffer> constantb2;

	//LINEAR
	ComPtr<ID3D11SamplerState> linearSampler;

	//POINT
	ComPtr<ID3D11SamplerState> pointSampler;

	//ONE INV_SRC_ALPHA
	ComPtr<ID3D11BlendState> blendState;

	DoubleRTV* dye;//LINEAR
	DoubleRTV* velocity;//LINEAR
	DoubleRTV* pressure;//POINT

	RenderTexture* divergence;//POINT
	RenderTexture* curl;//POINT
	RenderTexture* sunrays;//LINEAR
	RenderTexture* sunraysTemp;//LINEAR

	RenderTexture* finalOutput;

	Shader* advVelShader;
	Shader* advDenShader;
	Shader* blurShader;
	Shader* clearShader;
	Shader* curlShader;
	Shader* displayShader;
	Shader* divergenceShader;
	Shader* gradientSubtractShader;
	Shader* pressureShader;
	Shader* splatColor0;
	Shader* splatColor1;
	Shader* sunrayMaskShader;
	Shader* sunraysShader;
	Shader* vorticityShader;

	Shader* blurHVertex;
	Shader* blurVVertex;
	Shader* velocityVertex;
	Shader* displayVertex;

	float colorUpdateTimer = 0.0f;

	MyGame()
	{
		advVelShader = Shader::fromFile("Shaders\\AdvectionVelDissipation.hlsl", ShaderType::Pixel);
		advDenShader = Shader::fromFile("Shaders\\AdvectionDenDissipation.hlsl", ShaderType::Pixel);
		blurShader = Shader::fromFile("Shaders\\BlurShader.hlsl", ShaderType::Pixel);
		clearShader = Shader::fromFile("Shaders\\ClearShader.hlsl", ShaderType::Pixel);
		curlShader = Shader::fromFile("Shaders\\CurlShader.hlsl", ShaderType::Pixel);
		displayShader = Shader::fromFile("Shaders\\DisplayShader.hlsl", ShaderType::Pixel);
		divergenceShader = Shader::fromFile("Shaders\\DivergenceShader.hlsl", ShaderType::Pixel);
		gradientSubtractShader = Shader::fromFile("Shaders\\GradientSubtractShader.hlsl", ShaderType::Pixel);
		pressureShader = Shader::fromFile("Shaders\\PressureShader.hlsl", ShaderType::Pixel);
		splatColor0 = Shader::fromFile("Shaders\\SplatShaderColor0.hlsl", ShaderType::Pixel);
		splatColor1 = Shader::fromFile("Shaders\\SplatShaderColor1.hlsl", ShaderType::Pixel);
		sunrayMaskShader = Shader::fromFile("Shaders\\SunraysMaskShader.hlsl", ShaderType::Pixel);
		sunraysShader = Shader::fromFile("Shaders\\SunraysShader.hlsl", ShaderType::Pixel);
		vorticityShader = Shader::fromFile("Shaders\\VorticityShader.hlsl", ShaderType::Pixel);

		blurHVertex = Shader::fromFile("Shaders\\BlurVertexShaderH.hlsl", ShaderType::Vertex);
		blurVVertex = Shader::fromFile("Shaders\\BlurVertexShaderV.hlsl", ShaderType::Vertex);
		velocityVertex = Shader::fromFile("Shaders\\VelocityVertexShader.hlsl", ShaderType::Vertex);
		displayVertex = Shader::fromFile("Shaders\\DisplayVertexShader.hlsl", ShaderType::Vertex);

		//????????????blendState
		{
			D3D11_BLEND_DESC blendStateDesc = {};

			blendStateDesc.IndependentBlendEnable = false;

			blendStateDesc.RenderTarget[0].BlendEnable = true;
			blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

			blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

			blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;

			blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

			Graphics::device->CreateBlendState(&blendStateDesc, blendState.ReleaseAndGetAddressOf());
		}

		//????linearSampler
		{
			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			Graphics::device->CreateSamplerState(&sampDesc, linearSampler.ReleaseAndGetAddressOf());
		}

		//????pointSampler
		{
			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			Graphics::device->CreateSamplerState(&sampDesc, pointSampler.ReleaseAndGetAddressOf());
		}

		ID3D11SamplerState* samplers[2] = { linearSampler.Get(),pointSampler.Get() };

		Graphics::context->PSSetSamplers(0, 2, samplers);

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

			finalOutput = RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Transparent, false);

			//??????????????ConstantBuffer
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

		//????????????????ConstantBuffer
		{
			D3D11_BUFFER_DESC cbd = {};
			cbd.ByteWidth = sizeof(BufferDynamic);
			cbd.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
			cbd.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			Graphics::device->CreateBuffer(&cbd, nullptr, constantb2.ReleaseAndGetAddressOf());
		}

		ID3D11Buffer* buffers[2] = { constantb1.Get(),constantb2.Get() };

		Graphics::context->VSSetConstantBuffers(1, 2, buffers);

		Graphics::context->PSSetConstantBuffers(1, 2, buffers);

		Graphics::setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Mouse::addLeftDownEvent([this]()
			{
				std::cout << "DOWN\n";
				updatePointerDownData(-1, Mouse::getX(), Graphics::getHeight() - Mouse::getY());
			});

		Mouse::addMoveEvent([this]()
			{
				if (pointer.down)
				{
					updatePointerMoveData(Mouse::getX(), Graphics::getHeight() - Mouse::getY());
				}
			});

		Mouse::addLeftUpEvent([this]()
			{
				pointer.down = false;
			});


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

		delete advVelShader;
		delete advDenShader;
		delete blurShader;
		delete clearShader;
		delete curlShader;
		delete displayShader;
		delete divergenceShader;
		delete gradientSubtractShader;
		delete pressureShader;
		delete splatColor0;
		delete splatColor1;
		delete sunrayMaskShader;
		delete sunraysShader;
		delete vorticityShader;

		delete blurHVertex;
		delete blurVVertex;
		delete velocityVertex;
		delete displayVertex;
	}

	void blit(RenderTexture* const rt)
	{
		if (rt == nullptr)
		{
			Graphics::setDefRTV();
			Graphics::clearDefRTV(DirectX::Colors::Black);

			Shader::displayVShader->use();
			Shader::displayPShader->use();

			finalOutput->getTexture()->setSRV(0);

		}
		else
		{
			rt->setRTV();
		}

		Graphics::context->Draw(3, 0);

		ID3D11RenderTargetView* view = nullptr;
		Graphics::context->OMSetRenderTargets(1, &view, nullptr);

		ID3D11ShaderResourceView* resourceViews[2] = { nullptr,nullptr };
		Graphics::context->PSSetShaderResources(0, 2, resourceViews);

	}

	void updateColors(const float& dt)
	{
		colorUpdateTimer += dt * config.COLOR_UPDATE_SPEED;
		if (colorUpdateTimer >= 1.f)
		{
			colorUpdateTimer = 0;
			pointer.makeColorRandom();
		}
	}

	void applyInputs()
	{
		if (pointer.moved)
		{
			pointer.moved = false;
			const float dx = pointer.deltaX * config.SPLAT_FORCE;
			const float dy = pointer.deltaY * config.SPLAT_FORCE;
			splat(pointer.texcoordX, pointer.texcoordY, dx, dy, pointer.r, pointer.g, pointer.b);
		}
	}

	void splat(const float& x, const float& y, const float& dx, const float& dy, const float& r, const float& g, const float& b)
	{
		cBufferB2.color0 = DirectX::XMFLOAT3(dx, dy, 0.f);
		cBufferB2.color1 = DirectX::XMFLOAT3(r, g, b);
		cBufferB2.point = DirectX::XMFLOAT2(x, y);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		Graphics::context->Map(constantb2.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &cBufferB2, sizeof(BufferDynamic));
		Graphics::context->Unmap(constantb2.Get(), 0);

		Graphics::setViewport(velocity->width, velocity->height);
		velocityVertex->use();
		splatColor0->use();
		velocity->read()->getTexture()->setSRV(0);
		blit(velocity->write());
		velocity->swap();

		Graphics::setViewport(dye->width, dye->height);
		splatColor1->use();
		dye->read()->getTexture()->setSRV(0);
		blit(dye->write());
		dye->swap();
	}

	void step(const float& dt)
	{
		Graphics::setBlendState(StateCommon::blendReplace.Get());
		Graphics::setViewport(velocity->width, velocity->height);

		velocityVertex->use();
		curlShader->use();
		velocity->read()->getTexture()->setSRV(0);
		blit(curl);

		vorticityShader->use();
		velocity->read()->getTexture()->setSRV(0);
		curl->getTexture()->setSRV(1);
		blit(velocity->write());
		velocity->swap();

		divergenceShader->use();
		velocity->read()->getTexture()->setSRV(0);
		blit(divergence);

		clearShader->use();
		pressure->read()->getTexture()->setSRV(0);
		blit(pressure->write());
		pressure->swap();

		pressureShader->use();
		divergence->getTexture()->setSRV(0);
		for (int i = 0; i < config.PRESSURE_ITERATIONS; i++)
		{
			pressure->read()->getTexture()->setSRV(1);
			blit(pressure->write());
			pressure->swap();
		}

		gradientSubtractShader->use();
		pressure->read()->getTexture()->setSRV(0);
		velocity->read()->getTexture()->setSRV(1);
		blit(velocity->write());
		velocity->swap();

		advVelShader->use();
		velocity->read()->getTexture()->setSRV(0);
		velocity->read()->getTexture()->setSRV(1);
		blit(velocity->write());
		velocity->swap();

		Graphics::setViewport(dye->width, dye->height);

		advDenShader->use();
		velocity->read()->getTexture()->setSRV(0);
		dye->read()->getTexture()->setSRV(1);
		blit(dye->write());
		dye->swap();

	}

	void applySunrays(RenderTexture* const source, RenderTexture* const mask, RenderTexture* const destination)
	{
		Graphics::setBlendState(StateCommon::blendReplace.Get());
		velocityVertex->use();
		sunrayMaskShader->use();
		source->getTexture()->setSRV(0);
		Graphics::setViewport(mask->width, mask->height);
		blit(mask);

		sunraysShader->use();
		mask->getTexture()->setSRV(0);
		Graphics::setViewport(destination->width, destination->height);
		blit(destination);
	}

	void blur(RenderTexture* const target, RenderTexture* const temp, const int& iterations)
	{
		blurShader->use();
		for (int i = 0; i < iterations; i++)
		{
			blurVVertex->use();

			target->getTexture()->setSRV(0);
			blit(temp);

			blurHVertex->use();

			temp->getTexture()->setSRV(0);
			blit(target);
		}
	}

	void update(const float& dt) override
	{
		updateColors(dt);
		applyInputs();
		step(dt);
	}

	void render() override
	{
		applySunrays(dye->read(), dye->write(), sunrays);
		blur(sunrays, sunraysTemp, 1);

		Graphics::setBlendState(blendState.Get());
		Graphics::setViewport(Graphics::getWidth(), Graphics::getHeight());

		displayVertex->use();
		displayShader->use();
		dye->read()->getTexture()->setSRV(0);
		sunrays->getTexture()->setSRV(1);

		finalOutput->clearRTV(DirectX::Colors::Transparent);
		blit(finalOutput);

		blit(nullptr);
	}

	void updatePointerDownData(const int& id, const float& posX, const float& posY)
	{
		pointer.id = id;
		pointer.down = true;
		pointer.moved = false;
		pointer.texcoordX = posX / Graphics::getWidth();
		pointer.texcoordY = posY / Graphics::getHeight();
		pointer.prevTexcoordX = pointer.texcoordX;
		pointer.prevTexcoordY = pointer.texcoordY;
		pointer.deltaX = 0;
		pointer.deltaY = 0;
		pointer.makeColorRandom();
	}

	void updatePointerMoveData(const float& posX, const float& posY)
	{
		pointer.prevTexcoordX = pointer.texcoordX;
		pointer.prevTexcoordY = pointer.texcoordY;
		pointer.texcoordX = posX / Graphics::getWidth();
		pointer.texcoordY = posY / Graphics::getHeight();
		pointer.deltaX = pointer.texcoordX - pointer.prevTexcoordX;
		pointer.deltaY = correctDeltaY(pointer.texcoordY - pointer.prevTexcoordY);
		pointer.moved = fabsf(pointer.deltaX) > 0 || fabsf(pointer.deltaY) > 0;
	}

	void updatePointerUpData()
	{
		pointer.down = false;
	}

	float correctDeltaY(float delta)
	{
		const float aspectRatio = (float)Graphics::getWidth() / Graphics::getHeight();
		delta /= aspectRatio;
		return delta;
	}

};