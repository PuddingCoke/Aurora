#pragma once

#include<Aurora/Game.h>
#include<Aurora/A2D/SpriteBatch.h>
#include<Aurora/A3D/OrbitCamera.h>

#include"Scene.h"

const std::string assetPath = "D:/Assets/PBRRendering/";

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Scene scene;

	OrbitCamera camera;

	DepthStencilView* depthView;

	Buffer* lightBuffer;

	Shader* brdfGenPS;

	Shader* skyboxPS;

	RenderTexture* brdfTex;

	TextureCube* envCube;

	TextureCube* irradianceCube;

	TextureCube* prefilterCube;

	struct Light
	{
		DirectX::XMVECTOR lightPos;
		DirectX::XMFLOAT4 lightColor;
	}light;

	MyGame() :
		scene(assetPath + "DNA.obj"),
		depthView(new DepthStencilView(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, true)),
		camera({ 2,0,0 }, { 0,1,0 }),
		lightBuffer(new Buffer(sizeof(Light), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		brdfGenPS(new Shader("BRDFGenPS.hlsl", ShaderType::Pixel)),
		skyboxPS(new Shader("SkyboxPS.hlsl", ShaderType::Pixel)),
		brdfTex(new RenderTexture(512, 512, DXGI_FORMAT_R32G32_FLOAT)),
		envCube(new TextureCube(assetPath + "ParkinglotEnvHDR.dds")),
		irradianceCube(new TextureCube(assetPath + "ParkinglotDiffuseHDR.dds")),
		prefilterCube(new TextureCube(assetPath + "ParkinglotSpecularHDR.dds"))
	{
		light.lightColor = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);

		RenderAPI::get()->RSSetViewport(512, 512);
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->OMSetRTV({ brdfTex }, nullptr);

		RenderAPI::fullScreenVS->use();
		brdfGenPS->use();

		RenderAPI::get()->DrawQuad();
		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		camera.registerEvent();
	}

	~MyGame()
	{
		delete lightBuffer;
		delete depthView;
		delete brdfGenPS;
		delete skyboxPS;
		delete brdfTex;
		delete envCube;
		delete irradianceCube;
		delete prefilterCube;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);

		DirectX::XMVECTOR lightPos = Camera::getEye();
		lightPos = DirectX::XMVectorScale(DirectX::XMVector3Normalize(lightPos), 1.0f);

		light.lightPos = lightPos;

		memcpy(lightBuffer->map(0).pData, &light, sizeof(Light));
		lightBuffer->unmap(0);

	}

	void render()
	{
		depthView->clear(D3D11_CLEAR_DEPTH);
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::CadetBlue);
		RenderAPI::get()->OMSetDefRTV(depthView);

		RenderAPI::get()->PSSetBuffer({ Camera::getViewBuffer() }, 1);
		RenderAPI::get()->PSSetBuffer({ lightBuffer }, 3);
		RenderAPI::get()->PSSetSRV({ brdfTex,irradianceCube,prefilterCube }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		scene.draw();

		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RenderAPI::get()->PSSetSRV({ envCube }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::skyboxVS->use();
		skyboxPS->use();

		RenderAPI::get()->DrawCube();
	}


};