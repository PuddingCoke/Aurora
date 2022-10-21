﻿#pragma once

#include<Aurora/Game.h>
#include<Aurora/ComputeTexture3D.h>
#include<Aurora/RenderAPI.h>
#include<Aurora/A3D/OrbitCamera.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComputeTexture3D* mandelTexture;

	Shader* mandelCompute;

	Shader* modelVShader;

	Shader* modelPShader;

	Shader* rayCastVShader;

	Shader* rayCastPSHader;

	RenderTexture* modelCullFront;

	RenderTexture* modelCullBack;

	OrbitCamera camera;

	MyGame() :
		mandelTexture(new ComputeTexture3D(256, 256, 256, DXGI_FORMAT_R8G8B8A8_UNORM)),
		mandelCompute(Shader::fromFile("MandelbulbCompute.hlsl", ShaderType::Compute)),
		modelCullFront(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT)),
		modelCullBack(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT)),
		modelVShader(Shader::fromFile("ModelVShader.hlsl", ShaderType::Vertex)),
		modelPShader(Shader::fromFile("ModelPShader.hlsl", ShaderType::Pixel)),
		rayCastVShader(Shader::fromFile("RayCastVShader.hlsl", ShaderType::Vertex)),
		rayCastPSHader(Shader::fromFile("RayCastPShader.hlsl", ShaderType::Pixel)),
		camera({ 20,0,0 }, { 0,1,0 })
	{
		camera.registerEvent();

		RenderAPI::get()->CSSetUAV({ mandelTexture }, 0);
		mandelCompute->use();
		RenderAPI::get()->Dispatch(mandelTexture->width / 8, mandelTexture->height / 8, mandelTexture->depth / 8);
	}

	~MyGame()
	{
		delete mandelCompute;
		delete modelVShader;
		delete modelPShader;
		delete rayCastVShader;
		delete rayCastPSHader;
		delete mandelTexture;
		delete modelCullFront;
		delete modelCullBack;
	}

	void update(const float& dt) override
	{

	}

	void render()
	{
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		modelVShader->use();
		modelPShader->use();

		modelCullFront->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ modelCullFront }, nullptr);
		RenderAPI::get()->RSSetState(States::get()->rasterCullFront.Get());

		RenderAPI::get()->DrawCube();

		modelCullBack->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ modelCullBack }, nullptr);
		RenderAPI::get()->RSSetState(States::get()->rasterCullBack.Get());

		RenderAPI::get()->DrawCube();

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::White);
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetSRV({ mandelTexture,modelCullBack,modelCullFront }, 0);
		RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);

		rayCastVShader->use();
		rayCastPSHader->use();

		RenderAPI::get()->DrawCube();
	}


};