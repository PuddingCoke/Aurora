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

	Shader* curShader;

	MyGame() :
		mandelBulbPS(new Shader("MandelBulbPS.hlsl", ShaderType::Pixel)),
		sdfSphere(new Shader("SDFSphere.hlsl", ShaderType::Pixel))
	{
		curShader = mandelBulbPS;

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				if (curShader == mandelBulbPS)
				{
					curShader = sdfSphere;
				}
				else
				{
					curShader = mandelBulbPS;
				}
			});

	}

	~MyGame()
	{
		delete mandelBulbPS;
		delete sdfSphere;
	}

	void update(const float& dt) override
	{
	}

	void render()
	{
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::fullScreenVS->use();
		curShader->use();

		RenderAPI::get()->DrawQuad();
	}


};