#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/RenderAPI.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* mandelBrotPS;

	Buffer* simulationBuffer;

	float targetSize;

	struct SimulationParam
	{
		DirectX::XMFLOAT2 pos;
		float size;
		float aspectRatio;
	} param;

	MyGame() :
		mandelBrotPS(new Shader("MandelBrotPS.hlsl", ShaderType::Pixel)),
		param{ {0.f,0.f},10.f,Graphics::getAspectRatio() },
		targetSize(10.f)
	{

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					param.pos.x -= Mouse::getDX() * param.size * 0.005f;
					param.pos.y += Mouse::getDY() * param.size * 0.005f;
				}
			});

		Mouse::addScrollEvent([this]()
			{
				if (Mouse::getWheelDelta() < 0.f)
				{
					targetSize *= 1.2f;
				}
				else
				{
					targetSize *= 0.8f;
				}
			});

		simulationBuffer = new Buffer(sizeof(SimulationParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, &param, D3D11_CPU_ACCESS_WRITE);

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
	}

	~MyGame()
	{
		delete mandelBrotPS;
		delete simulationBuffer;
	}

	void imGUICall() override
	{
	}

	void update(const float& dt) override
	{
		param.size = Math::lerp(param.size, targetSize, 10.f * dt);

		memcpy(simulationBuffer->map().pData, &param, sizeof(SimulationParam));
		simulationBuffer->unmap();
	}

	void render()
	{
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetConstantBuffer({ simulationBuffer }, 1);

		RenderAPI::fullScreenVS->use();
		mandelBrotPS->use();

		RenderAPI::get()->DrawQuad();
	}


};