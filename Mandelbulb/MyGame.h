﻿#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/Utils/Math.h>
#include<Aurora/Resource/RenderTexture.h>
#include<Aurora/Resource/ComputeTexture.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* mandelBulbPS;

	Shader* displayPS;

	ConstantBuffer* simulationBuffer;

	//r frame rendered g random seed 
	ComputeTexture* randomTexture;

	float targetRadius;

	struct SimulationParam
	{
		float phi;
		float theta;
		float radius;
		float POWER;
	} param;

	RenderTexture* renderTexture;

	UINT frameIndex;

	MyGame() :
		renderTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16UN, 1, 1)),
		randomTexture(new ComputeTexture(1, 1, FMT::RG32F, FMT::RG32F, FMT::RG32F, 1, 1)),
		mandelBulbPS(new Shader("MandelBulbPS.hlsl", ShaderType::Pixel)),
		displayPS(new Shader("DisplayPS.hlsl", ShaderType::Pixel)),
		param{ Math::pi / 4.f + 0.4f,0.f,3.0f,8.f },
		frameIndex(0)
	{
		targetRadius = param.radius;

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					param.phi -= Mouse::getDY() * Graphics::getDeltaTime();
					param.theta += Mouse::getDX() * Graphics::getDeltaTime();
					param.phi = Math::clamp(param.phi, -Math::half_pi + 0.01f, Math::half_pi - 0.01f);

					frameIndex = 0;
				}
			});

		Mouse::addScrollEvent([this]()
			{
				targetRadius -= Mouse::getWheelDelta() * 0.1f;

				frameIndex = 0;
			});

		Graphics::setRecordConfig(1800, 60);

		simulationBuffer = new ConstantBuffer(sizeof(SimulationParam), D3D11_USAGE_DYNAMIC, &param);
	}

	~MyGame()
	{
		delete renderTexture;
		delete randomTexture;
		delete mandelBulbPS;
		delete displayPS;
		delete simulationBuffer;
	}

	void imGUICall() override
	{
		ImGui::SliderFloat("POWER", &param.POWER, 1.f, 8.f);
	}

	void update(const float& dt) override
	{
		/*param.theta += dt;
		param.phi = (Math::half_pi - 0.1f) * sinf(0.15f * Graphics::getSTime());
		param.POWER = 5.5 + 2.5f * sinf(0.25f * Graphics::getSTime());
		param.phi = Math::clamp(param.phi, -Math::half_pi + 0.01f, Math::half_pi - 0.01f);*/

		param.radius = targetRadius;

		//param.theta += dt;

		BufferUpdate::pushBufferUpdateParam(simulationBuffer, &param, sizeof(SimulationParam));
	}

	void render()
	{
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetBlendState(States::defBlendState);

		ImCtx::get()->PSSetConstantBuffer({ simulationBuffer }, 1);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);

		//ImCtx::get()->ClearRTV(renderTexture->getMip(0), DirectX::Colors::Black);

		ImCtx::get()->OMSetRTV({ renderTexture->getMip(0) }, nullptr);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		ImCtx::get()->PSSetSRV({ randomTexture }, 0);

		ImCtx::get()->BindShader(mandelBulbPS);

		frameIndex++;

		const float clearValue[4] = { frameIndex,Random::Float() * 1000.f,0.f,0.f };

		ImCtx::get()->ClearUAV(randomTexture->getMip(0), clearValue);

		ImCtx::get()->DrawQuad();

		/*for (UINT i = 0; i < 120; i++)
		{
			const float clearValue[4] = { i + 1,Random::Float() * 1000.f,0.f,0.f };

			ImCtx::get()->ClearUAV(randomTexture->getMip(0), clearValue);

			ImCtx::get()->DrawQuad();
		}*/

		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);

		ImCtx::get()->PSSetSRV({ renderTexture }, 0);

		ImCtx::get()->BindShader(displayPS);

		ImCtx::get()->DrawQuad();
	}


};