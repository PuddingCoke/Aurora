#pragma once

#include<Aurora/Game.h>
#include<Aurora/RenderAPI.h>
#include<Aurora/DoubleRTV.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* rayTracingPS;

	Shader* displayPS;

	Buffer* cameraParamBuffer;

	Buffer* temporalAccumulationBuffer;

	float targetRadius;

	DoubleRTV* swapTexture;

	struct CameraParam
	{
		float phi;
		float theta;
		float radius;
		float POWER;
	} cameraParam;

	struct TemporalAccumulationParam
	{
		unsigned int frameCount;
		float randomSeed;
		DirectX::XMFLOAT2 padding;
	}temporalAccumulationParam;

	MyGame() :
		rayTracingPS(new Shader("RayTracingPS.hlsl", ShaderType::Pixel)),
		displayPS(new Shader("DisplayPS.hlsl", ShaderType::Pixel)),
		swapTexture(new DoubleRTV(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT)),
		cameraParam{ 0.25f,0.0f,12.0f,0.1f },
		temporalAccumulationParam{ 0u,0.f,{} }
	{
		targetRadius = cameraParam.radius;

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					cameraParam.phi -= Mouse::getDY() * Graphics::getDeltaTime();
					cameraParam.theta += Mouse::getDX() * Graphics::getDeltaTime();
					cameraParam.phi = Math::clamp(cameraParam.phi, 0.01f, Math::half_pi - 0.01f);
				}
			});

		Mouse::addScrollEvent([this]()
			{
				targetRadius -= Mouse::getWheelDelta() * 1.f;
			});

		cameraParamBuffer = new Buffer(sizeof(CameraParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, &cameraParam, D3D11_CPU_ACCESS_WRITE);
		temporalAccumulationBuffer = new Buffer(sizeof(TemporalAccumulationParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE);
	}

	~MyGame()
	{
		delete rayTracingPS;
		delete displayPS;
		delete cameraParamBuffer;
		delete temporalAccumulationBuffer;
		delete swapTexture;
	}

	void imGUICall() override
	{
	}

	void update(const float& dt) override
	{
		cameraParam.radius = Math::lerp(cameraParam.radius, targetRadius, 10.f * dt);
		//cameraParam.theta += dt * 0.5f;

		memcpy(cameraParamBuffer->map(0).pData, &cameraParam, sizeof(CameraParam));
		cameraParamBuffer->unmap(0);
	}

	void render()
	{
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->PSSetConstantBuffer({ cameraParamBuffer,temporalAccumulationBuffer }, 1);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::fullScreenVS->use();

		temporalAccumulationParam.frameCount = 0;
		swapTexture->read()->clearRTV(DirectX::Colors::Black);
		swapTexture->write()->clearRTV(DirectX::Colors::Black);

		for (unsigned int i = 0; i < 20; i++)
		{
			temporalAccumulationParam.frameCount++;
			temporalAccumulationParam.randomSeed = Random::Float() * 50.f;

			memcpy(temporalAccumulationBuffer->map(0).pData, &temporalAccumulationParam, sizeof(TemporalAccumulationParam));
			temporalAccumulationBuffer->unmap(0);

			RenderAPI::get()->OMSetRTV({ swapTexture->write() }, nullptr);
			RenderAPI::get()->PSSetSRV({ swapTexture->read() }, 0);

			rayTracingPS->use();

			RenderAPI::get()->DrawQuad();
			swapTexture->swap();
		}

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->PSSetSRV({ swapTexture->read() }, 0);

		displayPS->use();

		RenderAPI::get()->DrawQuad();
	}


};