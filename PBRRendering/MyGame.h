#pragma once

#include<Aurora/Game.h>
#include<Aurora/Camera/OrbitCamera.h>
#include<Aurora/EngineAPI/ResourceEssential.h>

#include"Scene.h"

const std::string assetPath = "E:/Assets/PBRRendering/";

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	Scene scene;

	OrbitCamera camera;

	DepthTexture* depthTexture;

	ConstantBuffer* lightBuffer;

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
		depthTexture(new DepthTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::D32F, true)),
		camera({ 2,0,0 }, { 0,1,0 }),
		lightBuffer(new ConstantBuffer(sizeof(Light), D3D11_USAGE_DYNAMIC)),
		brdfGenPS(new Shader("BRDFGenPS.hlsl", ShaderType::Pixel)),
		skyboxPS(new Shader("SkyboxPS.hlsl", ShaderType::Pixel)),
		brdfTex(new RenderTexture(512, 512, FMT::RG32F)),
		envCube(new TextureCube(assetPath + "ParkinglotEnvHDR.dds")),
		irradianceCube(new TextureCube(assetPath + "ParkinglotDiffuseHDR.dds")),
		prefilterCube(new TextureCube(assetPath + "ParkinglotSpecularHDR.dds"))
	{
		light.lightColor = DirectX::XMFLOAT4(1.f, 1.f, 1.f, 1.f);

		ImCtx::get()->RSSetViewport(512, 512);
		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		ImCtx::get()->OMSetRTV({ brdfTex->getMip(0) }, nullptr);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(brdfGenPS);

		ImCtx::get()->DrawQuad();
		ImCtx::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 0.1f, 256.f);

		camera.registerEvent();
	}

	~MyGame()
	{
		delete lightBuffer;
		delete depthTexture;
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

		memcpy(ImCtx::get()->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &light, sizeof(Light));
		ImCtx::get()->Unmap(lightBuffer, 0);
	}

	void render()
	{
		ImCtx::get()->ClearDSV(depthTexture, D3D11_CLEAR_DEPTH);
		ImCtx::get()->ClearDefRTV(DirectX::Colors::CadetBlue);
		ImCtx::get()->OMSetDefRTV(depthTexture);

		ImCtx::get()->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);
		ImCtx::get()->PSSetConstantBuffer({ lightBuffer }, 3);
		ImCtx::get()->PSSetSRV({ brdfTex,irradianceCube,prefilterCube }, 0);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		scene.draw();

		ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ImCtx::get()->PSSetSRV({ envCube }, 0);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::skyboxVS);
		ImCtx::get()->BindShader(skyboxPS);

		ImCtx::get()->DrawCube();
	}


};