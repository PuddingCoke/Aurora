#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/ImCtx.h>

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
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16UN)),
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

		memcpy(ImCtx::get()->Map(cameraParamBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &cameraParam, sizeof(CameraParam));
		ImCtx::get()->Unmap(cameraParamBuffer, 0);
	}

	void render()
	{
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetBlendState(States::defBlendState);

		ImCtx::get()->PSSetConstantBuffer({ cameraParamBuffer,temporalAccumulationBuffer }, 1);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(ImCtx::fullScreenVS);

		temporalAccumulationParam.frameCount = 0;

		ImCtx::get()->ClearRTV(renderTexture->getMip(0), DirectX::Colors::Black);

		ImCtx::get()->OMSetRTV({ renderTexture->getMip(0) }, nullptr);

		ImCtx::get()->BindShader(rayTracingPS);

		for (unsigned int i = 0; i < 10; i++)
		{
			temporalAccumulationParam.frameCount++;
			temporalAccumulationParam.randomSeed = Random::Float() * 50.f;

			memcpy(ImCtx::get()->Map(temporalAccumulationBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &temporalAccumulationParam, sizeof(TemporalAccumulationParam));
			ImCtx::get()->Unmap(temporalAccumulationBuffer, 0);

			ImCtx::get()->DrawQuad();
		}

		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);

		ImCtx::get()->PSSetSRV({ renderTexture }, 0);

		ImCtx::get()->BindShader(displayPS);

		ImCtx::get()->DrawQuad();
	}


};