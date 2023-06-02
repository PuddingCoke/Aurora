#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/RenderAPI.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* rayTracingPS;

	Shader* displayPS;

	ConstantBuffer* cameraParamBuffer;

	ConstantBuffer* temporalAccumulationBuffer;

	float targetRadius;

	RenderTexture* renderTexture;

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
		rayTracingPS(new Shader(Utils::getRootFolder() + "RayTracingPS.cso", ShaderType::Pixel)),
		displayPS(new Shader("DisplayPS.hlsl", ShaderType::Pixel)),
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_UNORM)),
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

		//prev*(1.0-1.0/frameCount)+cur*(1.0/frameCount)

		cameraParamBuffer = new ConstantBuffer(sizeof(CameraParam), D3D11_USAGE_DYNAMIC, &cameraParam);
		temporalAccumulationBuffer = new ConstantBuffer(sizeof(TemporalAccumulationParam), D3D11_USAGE_DYNAMIC);
	}

	~MyGame()
	{
		delete rayTracingPS;
		delete displayPS;
		delete cameraParamBuffer;
		delete temporalAccumulationBuffer;
		delete renderTexture;
	}

	void imGUICall() override
	{
	}

	void update(const float& dt) override
	{
		cameraParam.radius = Math::lerp(cameraParam.radius, targetRadius, 10.f * dt);
		//cameraParam.theta += dt * 0.5f;

		memcpy(cameraParamBuffer->map().pData, &cameraParam, sizeof(CameraParam));
		cameraParamBuffer->unmap();
	}

	void render()
	{
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(States::defBlendState);

		RenderAPI::get()->PSSetConstantBuffer({ cameraParamBuffer,temporalAccumulationBuffer }, 1);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::fullScreenVS->use();

		temporalAccumulationParam.frameCount = 0;

		renderTexture->clearRTV(DirectX::Colors::Black);

		RenderAPI::get()->OMSetRTV({ renderTexture }, nullptr);

		rayTracingPS->use();

		for (unsigned int i = 0; i < 10; i++)
		{
			temporalAccumulationParam.frameCount++;
			temporalAccumulationParam.randomSeed = Random::Float() * 50.f;

			memcpy(temporalAccumulationBuffer->map().pData, &temporalAccumulationParam, sizeof(TemporalAccumulationParam));
			temporalAccumulationBuffer->unmap();

			RenderAPI::get()->DrawQuad();
		}

		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->PSSetSRV({ renderTexture }, 0);

		displayPS->use();

		RenderAPI::get()->DrawQuad();
	}


};