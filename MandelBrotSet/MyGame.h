#pragma once

#include<Aurora/Game.h>
#include<Aurora/Utils/Math.h>
#include<Aurora/EngineAPI/ImCtx.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* mandelBrotPS;

	ConstantBuffer* simulationBuffer;

	double targetSize;

	struct SimulationParam
	{
		double x;
		double y;
		double size;
		double aspectRatio;
	} param;

	MyGame() :
		mandelBrotPS(new Shader("MandelBrotPS.hlsl", ShaderType::Pixel)),
		param{ 0.0,0.0,7.0,(double)Graphics::getAspectRatio() },
		targetSize(6.0)
	{

		Mouse::addMoveEvent([this]()
			{
				if (Mouse::getLeftDown())
				{
					param.x -= (double)Mouse::getDX() * param.size * 0.005;
					param.y += (double)Mouse::getDY() * param.size * 0.005;
				}
			});

		Mouse::addScrollEvent([this]()
			{
				if (Mouse::getWheelDelta() < 0.f)
				{
					targetSize *= 1.2;
				}
				else
				{
					targetSize *= 0.8;
				}

				/*if (targetSize < 2e-6)
				{
					targetSize = 2e-6;
				}
				else if (targetSize > 6.0)
				{
					targetSize = 6.0;
				}*/
			});

		simulationBuffer = new ConstantBuffer(sizeof(SimulationParam), D3D11_USAGE_DYNAMIC, &param);

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetBlendState(nullptr);
		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
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
		param.size = param.size * (1.0 - 10.0 * dt) + targetSize * 10.0 * dt;

		memcpy(ImCtx::get()->Map(simulationBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &param, sizeof(SimulationParam));
		ImCtx::get()->Unmap(simulationBuffer, 0);
	}

	void render()
	{
		ImCtx::get()->OMSetDefRTV(nullptr);
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->PSSetConstantBuffer({ simulationBuffer }, 1);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(mandelBrotPS);

		ImCtx::get()->DrawQuad();
	}


};