#pragma once
#include<iostream>

#include<Aurora/Game.h>
#include<Aurora/Mouse.h>
#include<Aurora/Event.h>
#include<Aurora/States.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Color.h>
#include<Aurora/DoubleRTV.h>
#include<Aurora/Timer.h>

#include"Config.h"

class MyGame :public Game
{
public:

	struct Pointer
	{
		float texcoordX = 0;
		float texcoordY = 0;
		float prevTexcoordX = 0;
		float prevTexcoordY = 0;
		float deltaX = 0;
		float deltaY = 0;
		bool down = false;
		bool moved = false;
		float r = 30;
		float g = 0;
		float b = 300;

		void makeColorRandom()
		{
			const Color c = Color::HSVtoRGB({ Random::Float(),1.f,1.f });
			r = c.r * 0.15f;
			g = c.g * 0.15f;
			b = c.b * 0.15f;
		}

	} pointer;

	//USAGE IMMUTABLE
	Buffer* simulationParamBuffer;

	//USAGE DYNAMIC
	Buffer* splatParamBuffer;

	//ONE INV_SRC_ALPHA
	ComPtr<ID3D11BlendState> blendState;

	DoubleRTV* dye;//LINEAR
	DoubleRTV* velocity;//LINEAR
	DoubleRTV* pressure;//POINT

	RenderTexture* divergence;//POINT
	RenderTexture* curl;//POINT
	RenderTexture* sunrays;//LINEAR
	RenderTexture* sunraysTemp;//LINEAR

	Shader* advVelShader;
	Shader* advDenShader;
	Shader* clearShader;
	Shader* curlShader;
	Shader* displayShader;
	Shader* divergenceShader;
	Shader* gradientSubtractShader;
	Shader* pressureShader;
	Shader* splatVelocityShader;
	Shader* splatColorShader;
	Shader* sunrayMaskShader;
	Shader* sunraysShader;
	Shader* vorticityShader;

	Shader* blurHShader;
	Shader* blurVShader;

	Timer colorUpdateTimer;

	MyGame() :
		colorUpdateTimer(1.f),
		advVelShader(new Shader("Shaders\\AdvectionVelDissipation.hlsl", ShaderType::Pixel)),
		advDenShader(new Shader("Shaders\\AdvectionDenDissipation.hlsl", ShaderType::Pixel)),
		clearShader(new Shader("Shaders\\ClearShader.hlsl", ShaderType::Pixel)),
		curlShader(new Shader("Shaders\\CurlShader.hlsl", ShaderType::Pixel)),
		displayShader(new Shader("Shaders\\DisplayShader.hlsl", ShaderType::Pixel)),
		divergenceShader(new Shader("Shaders\\DivergenceShader.hlsl", ShaderType::Pixel)),
		gradientSubtractShader(new Shader("Shaders\\GradientSubtractShader.hlsl", ShaderType::Pixel)),
		pressureShader(new Shader("Shaders\\PressureShader.hlsl", ShaderType::Pixel)),
		splatVelocityShader(new Shader("Shaders\\SplatVelocityShader.hlsl", ShaderType::Pixel)),
		splatColorShader(new Shader("Shaders\\SplatColorShader.hlsl", ShaderType::Pixel)),
		sunrayMaskShader(new Shader("Shaders\\SunraysMaskShader.hlsl", ShaderType::Pixel)),
		sunraysShader(new Shader("Shaders\\SunraysShader.hlsl", ShaderType::Pixel)),
		vorticityShader(new Shader("Shaders\\VorticityShader.hlsl", ShaderType::Pixel)),
		blurHShader(new Shader("Shaders\\BlurShaderHBlur.hlsl", ShaderType::Pixel)),
		blurVShader(new Shader("Shaders\\BlurShaderVBlur.hlsl", ShaderType::Pixel)),
		splatParamBuffer(new Buffer(sizeof(SplatParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE))
	{
		//创建自定义的blendState
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

			Renderer::device->CreateBlendState(&blendStateDesc, blendState.ReleaseAndGetAddressOf());
		}

		{
			const DirectX::XMINT2 simRes = getResolution(SimulationConfig::SIM_RESOLUTION);
			const DirectX::XMINT2 dyeRes = getResolution(SimulationConfig::DYE_RESOLUTION);
			const DirectX::XMINT2 sunRes = getResolution(SimulationConfig::SUNRAYS_RESOLUTION);

			dye = DoubleRTV::create(dyeRes.x, dyeRes.y, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
			velocity = DoubleRTV::create(simRes.x, simRes.y, DXGI_FORMAT::DXGI_FORMAT_R16G16_FLOAT);
			pressure = DoubleRTV::create(simRes.x, simRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT);
			divergence = new RenderTexture(simRes.x, simRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, DirectX::Colors::Transparent);
			curl = new RenderTexture(simRes.x, simRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, DirectX::Colors::Transparent);
			sunrays = new RenderTexture(sunRes.x, sunRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, DirectX::Colors::Transparent);
			sunraysTemp = new RenderTexture(sunRes.x, sunRes.y, DXGI_FORMAT::DXGI_FORMAT_R16_FLOAT, DirectX::Colors::Transparent);

			//创建数据不变的ConstantBuffer
			{
				struct ConstantBuffer
				{
					DirectX::XMFLOAT2 screenTexelSize;
					DirectX::XMFLOAT2 simTexelSize;
					DirectX::XMFLOAT2 sunTexelSize;
					float velocity_dissipation;
					float density_dissipation;
					float value;
					float aspectRatio;
					float curl;
					float radius;
					float weight;
					DirectX::XMFLOAT3 v0;
				} constantBuffer{};

				constantBuffer.screenTexelSize = DirectX::XMFLOAT2(1.f / Graphics::getWidth(), 1.f / Graphics::getHeight());
				constantBuffer.simTexelSize = DirectX::XMFLOAT2(1.f / simRes.x, 1.f / simRes.y);
				constantBuffer.sunTexelSize = DirectX::XMFLOAT2(1.f / sunRes.x, 1.f / sunRes.y);
				constantBuffer.velocity_dissipation = SimulationConfig::VELOCITY_DISSIPATION;
				constantBuffer.density_dissipation = SimulationConfig::DENSITY_DISSIPATION;
				constantBuffer.value = SimulationConfig::PRESSURE;
				constantBuffer.aspectRatio = Graphics::getAspectRatio();
				constantBuffer.curl = SimulationConfig::CURL;
				constantBuffer.radius = SimulationConfig::SPLAT_RADIUS / 100.f * Graphics::getAspectRatio();
				constantBuffer.weight = SimulationConfig::SUNRAYS_WEIGHT;

				simulationParamBuffer = new Buffer(sizeof(ConstantBuffer), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &constantBuffer);
			}
		}

		RenderAPI::get()->VSSetBuffer({ simulationParamBuffer }, 1);
		RenderAPI::get()->PSSetBuffer({ simulationParamBuffer,splatParamBuffer }, 1);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Mouse::addLeftDownEvent([this]()
			{
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

		RenderAPI::get()->PSSetSampler({ States::linearClampSampler,States::pointClampSampler }, 0);

		RenderAPI::fullScreenVS->use();
	}

	~MyGame()
	{
		delete simulationParamBuffer;
		delete splatParamBuffer;

		delete dye;
		delete velocity;
		delete pressure;

		delete divergence;
		delete curl;
		delete sunrays;
		delete sunraysTemp;

		delete advVelShader;
		delete advDenShader;
		delete clearShader;
		delete curlShader;
		delete displayShader;
		delete divergenceShader;
		delete gradientSubtractShader;
		delete pressureShader;
		delete splatVelocityShader;
		delete splatColorShader;
		delete sunrayMaskShader;
		delete sunraysShader;
		delete vorticityShader;

		delete blurHShader;
		delete blurVShader;
	}

	void updateColors(const float& dt)
	{
		if (colorUpdateTimer.update(dt * SimulationConfig::COLOR_UPDATE_SPEED))
		{
			pointer.makeColorRandom();
		}
	}

	void applyInputs()
	{
		if (pointer.moved)
		{
			pointer.moved = false;
			const float dx = pointer.deltaX * SimulationConfig::SPLAT_FORCE;
			const float dy = pointer.deltaY * SimulationConfig::SPLAT_FORCE;
			splat(pointer.texcoordX, pointer.texcoordY, dx, dy, pointer.r, pointer.g, pointer.b);
		}
	}

	void splat(const float& x, const float& y, const float& dx, const float& dy, const float& r, const float& g, const float& b)
	{
		splatParam.mousePos = DirectX::XMFLOAT2(x, y);
		splatParam.mouseDelta = DirectX::XMFLOAT2(dx, dy);
		splatParam.color = DirectX::XMFLOAT4(r, g, b, 1.0);

		memcpy(splatParamBuffer->map(0).pData, &splatParam, sizeof(SplatParam));
		splatParamBuffer->unmap(0);

		RenderAPI::get()->RSSetViewport(velocity->width, velocity->height);
		RenderAPI::get()->OMSetRTV({ velocity->write() }, nullptr);
		splatVelocityShader->use();
		RenderAPI::get()->PSSetSRV({ velocity->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocity->swap();

		RenderAPI::get()->RSSetViewport(dye->width, dye->height);
		RenderAPI::get()->OMSetRTV({ dye->write() }, nullptr);
		splatColorShader->use();
		RenderAPI::get()->PSSetSRV({ dye->read() }, 0);
		RenderAPI::get()->DrawQuad();
		dye->swap();
	}

	void step(const float& dt)
	{
		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->RSSetViewport(velocity->width, velocity->height);

		RenderAPI::get()->OMSetRTV({ curl }, nullptr);
		curlShader->use();
		RenderAPI::get()->PSSetSRV({ velocity->read() }, 0);
		RenderAPI::get()->DrawQuad();

		RenderAPI::get()->OMSetRTV({ velocity->write() }, nullptr);
		vorticityShader->use();
		RenderAPI::get()->PSSetSRV({ velocity->read(),curl }, 0);
		RenderAPI::get()->DrawQuad();
		velocity->swap();

		RenderAPI::get()->OMSetRTV({ divergence }, nullptr);
		divergenceShader->use();
		RenderAPI::get()->PSSetSRV({ velocity->read() }, 0);
		RenderAPI::get()->DrawQuad();

		RenderAPI::get()->OMSetRTV({ pressure->write() }, nullptr);
		clearShader->use();
		RenderAPI::get()->PSSetSRV({ pressure->read() }, 0);
		RenderAPI::get()->DrawQuad();
		pressure->swap();

		pressureShader->use();
		for (int i = 0; i < SimulationConfig::PRESSURE_ITERATIONS; i++)
		{
			RenderAPI::get()->OMSetRTV({ pressure->write() }, nullptr);
			RenderAPI::get()->PSSetSRV({ pressure->read(),divergence }, 0);
			RenderAPI::get()->DrawQuad();
			pressure->swap();
		}

		RenderAPI::get()->OMSetRTV({ velocity->write() }, nullptr);
		gradientSubtractShader->use();
		RenderAPI::get()->PSSetSRV({ pressure->read(),velocity->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocity->swap();

		RenderAPI::get()->OMSetRTV({ velocity->write() }, nullptr);
		advVelShader->use();
		RenderAPI::get()->PSSetSRV({ velocity->read() }, 0);
		RenderAPI::get()->DrawQuad();
		velocity->swap();

		RenderAPI::get()->RSSetViewport(dye->width, dye->height);

		RenderAPI::get()->OMSetRTV({ dye->write() }, nullptr);
		advDenShader->use();
		RenderAPI::get()->PSSetSRV({ velocity->read(),dye->read() }, 0);
		RenderAPI::get()->DrawQuad();
		dye->swap();
	}

	void applySunrays(RenderTexture* const source, RenderTexture* const mask, RenderTexture* const destination)
	{
		RenderAPI::get()->OMSetRTV({ mask }, nullptr);
		RenderAPI::get()->OMSetBlendState(nullptr);
		sunrayMaskShader->use();
		RenderAPI::get()->PSSetSRV({ source }, 0);
		RenderAPI::get()->RSSetViewport(mask->getWidth(), mask->getHeight());
		RenderAPI::get()->DrawQuad();

		RenderAPI::get()->OMSetRTV({ destination }, nullptr);
		sunraysShader->use();
		RenderAPI::get()->PSSetSRV({ mask }, 0);
		RenderAPI::get()->RSSetViewport(destination->getWidth(), destination->getHeight());
		RenderAPI::get()->DrawQuad();
	}

	void blur(RenderTexture* const target, RenderTexture* const temp, const int& iterations)
	{
		for (int i = 0; i < iterations; i++)
		{
			blurHShader->use();
			RenderAPI::get()->OMSetRTV({ temp }, nullptr);
			RenderAPI::get()->PSSetSRV({ target }, 0);
			RenderAPI::get()->DrawQuad();

			blurVShader->use();

			RenderAPI::get()->OMSetRTV({ target }, nullptr);
			RenderAPI::get()->PSSetSRV({ temp }, 0);
			RenderAPI::get()->DrawQuad();
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

		RenderAPI::get()->OMSetBlendState(blendState.Get());

		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);
		displayShader->use();
		RenderAPI::get()->PSSetSRV({ dye->read(),sunrays }, 0);
		RenderAPI::get()->DrawQuad();
	}

	void updatePointerDownData(const int& id, const float& posX, const float& posY)
	{
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
		pointer.deltaY = (pointer.texcoordY - pointer.prevTexcoordY) / Graphics::getAspectRatio();
		pointer.moved = fabsf(pointer.deltaX) > 0 || fabsf(pointer.deltaY) > 0;
	}

};