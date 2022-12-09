#pragma once

#include<Aurora/Game.h>
#include<Aurora/ComputeTexture3D.h>
#include<Aurora/RenderAPI.h>
#include<Aurora/A3D/OrbitCamera.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* mandelBulbPS;

	Shader* sdfSphere;

	Buffer* simulationBuffer;

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
		sdfSphere(new Shader("SDFSphere.hlsl", ShaderType::Pixel)),
		param{ 0.f,0.f,3.0f,8.f }
	{
		targetRadius = param.radius;

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					param.phi -= Mouse::getDY() * Graphics::getDeltaTime();
					param.theta -= Mouse::getDX() * Graphics::getDeltaTime();
					param.phi = Math::clamp(param.phi, -Math::half_pi + 0.05f, Math::half_pi - 0.05f);
				}
			});

		Mouse::addScrollEvent([this]()
			{
				targetRadius -= Mouse::getWheelDelta() * 0.1f;
			});

		simulationBuffer = new Buffer(sizeof(SimulationParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, &param, D3D11_CPU_ACCESS_WRITE);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
	}

	~MyGame()
	{
		delete mandelBulbPS;
		delete sdfSphere;
		delete simulationBuffer;
	}

	void update(const float& dt) override
	{
		/*param.theta += dt;
		param.phi = (Math::half_pi - 0.05) * sinf(Graphics::getSTime());
		param.POWER = 5.5 + 2.5f * sinf(Graphics::getSTime());*/

		if (Keyboard::getKeyDown(Keyboard::Q))
		{
			param.POWER -= dt * 5.f;
		}

		if (Keyboard::getKeyDown(Keyboard::E))
		{
			param.POWER += dt * 5.f;
		}

		param.POWER = Math::clamp(param.POWER, 3.f, 8.f);
		param.radius = Math::lerp(param.radius, targetRadius, 10.f * dt);

		memcpy(simulationBuffer->map(0).pData, &param, sizeof(SimulationParam));
		simulationBuffer->unmap(0);
	}

	void render()
	{
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetBuffer({ simulationBuffer }, 1);

		RenderAPI::fullScreenVS->use();
		mandelBulbPS->use();

		RenderAPI::get()->DrawQuad();
	}


};