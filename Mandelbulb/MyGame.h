﻿#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/Utils/Math.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* mandelBulbPS;

	ConstantBuffer* simulationBuffer;

	float targetRadius;

	struct SimulationParam
	{
		float phi;
		float theta;
		float radius;
		float POWER;
	} param;

	MyGame() :
		mandelBulbPS(new Shader("MandelBulbPS.hlsl", ShaderType::Pixel)),
		param{ 0.f,0.f,3.0f,8.f }
	{
		targetRadius = param.radius;

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					param.phi -= Mouse::getDY() * Graphics::getDeltaTime();
					param.theta += Mouse::getDX() * Graphics::getDeltaTime();
					param.phi = Math::clamp(param.phi, -Math::half_pi + 0.01f, Math::half_pi - 0.01f);
				}
			});

		Mouse::addScrollEvent([this]()
			{
				targetRadius -= Mouse::getWheelDelta() * 0.1f;
			});

		simulationBuffer = new ConstantBuffer(sizeof(SimulationParam), D3D11_USAGE_DYNAMIC, &param);
	}

	~MyGame()
	{
		delete mandelBulbPS;
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

		param.radius = Math::lerp(param.radius, targetRadius, 10.f * dt);

		BufferUpdate::pushBufferUpdateParam(simulationBuffer, &param, sizeof(SimulationParam));
	}

	void render()
	{
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);

		ImCtx::get()->OMSetDefRTV(nullptr);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		ImCtx::get()->PSSetConstantBuffer({ simulationBuffer }, 1);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(mandelBulbPS);

		ImCtx::get()->DrawQuad();
	}


};