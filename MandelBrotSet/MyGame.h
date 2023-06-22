#pragma once

#include<Aurora/Game.h>
#include<Aurora/EngineAPI/ImCtx.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Shader* mandelBrotPS;

	ConstantBuffer* simulationBuffer;

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
		param.size = Math::lerp(param.size, targetSize, 10.f * dt);

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