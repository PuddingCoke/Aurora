#pragma once

#include<Aurora/Game.h>
#include<Aurora/Resource/ResourceDepthTexture.h>
#include<Aurora/Resource/TextureCube.h>
#include<Aurora/Camera/FPSCamera.h>
#include<Aurora/Effect/BloomEffect.h>
#include<Aurora/Effect/HBAOEffect.h>

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	ComPtr<ID3D11SamplerState> borderSampler;

	FPSCamera camera;

	ResourceDepthTexture* resDepthTexture;

	Scene* mainScene;

	//Scene* curtainScene;

	RenderTexture* gBaseColor;

	RenderTexture* gPosition;

	RenderTexture* gNormal;

	RenderTexture* gRoughnessMetallic;

	RenderTexture* originTexture;

	RenderTexture* reflectTexture;

	Shader* deferredVS;

	//有贴图
	Shader* deferredPSHasTex;

	//无贴图
	Shader* deferredPSNoTex;

	//透明材质
	Shader* deferredPSTrans;

	Shader* debugPS;

	Shader* deferredFinal;

	Shader* skyboxPS;

	Shader* screenSpaceReflection;

	TextureCube* skybox;

	BloomEffect bloomEffect;

	HBAOEffect hbaoEffect;

	MyGame() :
		camera({ 0,10,0 }, { 1,0,0 }, { 0,1,0 }, 10),
		resDepthTexture(new ResourceDepthTexture(Graphics::getWidth(), Graphics::getHeight())),
		mainScene(new Scene("E:/Assets/SuperSponza/NewSponza_Main_glTF_002.gltf")),
		//curtainScene(new Scene("E:/Assets/SuperSponza/NewSponza_Curtains_glTF.gltf")),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN)),
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA32F)),
		gNormal(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA32F)),
		gRoughnessMetallic(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F)),
		reflectTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F)),
		deferredVS(new Shader("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPSHasTex(new Shader("DeferredPSHasTex.hlsl", ShaderType::Pixel)),
		deferredPSNoTex(new Shader("DeferredPSNoTex.hlsl", ShaderType::Pixel)),
		deferredPSTrans(new Shader("DeferredPSTrans.hlsl", ShaderType::Pixel)),
		debugPS(new Shader("DebugPS.hlsl", ShaderType::Pixel)),
		deferredFinal(new Shader("DeferredFinal.hlsl", ShaderType::Pixel)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		skybox(new TextureCube("E:/Assets/SpaceShip/360-Space-Panorama-III.hdr", 2048)),
		skyboxPS(new Shader("SkyboxPS.hlsl", ShaderType::Pixel)),
		screenSpaceReflection(new Shader("SSR.hlsl", ShaderType::Pixel))
	{
		bloomEffect.setThreshold(1.f);
		bloomEffect.setIntensity(1.f);
		bloomEffect.applyChange();

		{
			D3D11_INPUT_ELEMENT_DESC layout[5] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			GraphicsDevice::get()->createInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(deferredVS), inputLayout.ReleaseAndGetAddressOf());
		}

		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 0.1f, 35.f);
	}

	~MyGame()
	{
		delete mainScene;
		//delete curtainScene;

		delete resDepthTexture;

		delete gBaseColor;
		delete gPosition;
		delete gNormal;
		delete gRoughnessMetallic;
		delete originTexture;

		delete skybox;

		delete deferredVS;
		delete deferredPSHasTex;
		delete deferredPSNoTex;
		delete deferredPSTrans;
		delete debugPS;
		delete deferredFinal;
		delete skyboxPS;
		delete screenSpaceReflection;
		delete reflectTexture;
	}

	void imGUICall() override
	{
		bloomEffect.imGUIEffectModifier();
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		ImCtx::get()->IASetInputLayout(inputLayout.Get());
		ImCtx::get()->PSSetSampler({ States::anisotropicWrapSampler,States::linearClampSampler }, 0);

		ImCtx::get()->ClearDSV(resDepthTexture, D3D11_CLEAR_DEPTH);
		ImCtx::get()->ClearRTV(gBaseColor->getMip(0), DirectX::Colors::Black);
		ImCtx::get()->ClearRTV(gPosition->getMip(0), DirectX::Colors::Black);
		ImCtx::get()->ClearRTV(gNormal->getMip(0), DirectX::Colors::Black);
		ImCtx::get()->ClearRTV(gRoughnessMetallic->getMip(0), DirectX::Colors::Black);

		ImCtx::get()->OMSetRTV({ gBaseColor->getMip(0),gPosition->getMip(0),gNormal->getMip(0),gRoughnessMetallic->getMip(0) }, resDepthTexture);

		mainScene->draw(deferredVS, deferredPSHasTex, deferredPSNoTex, deferredPSTrans);

		//ImCtx::get()->RSSetState(States::rasterCullNone);
		//curtainScene->draw(deferredVS, deferredPSHasTex, deferredPSNoTex, deferredPSTrans);
		//ImCtx::get()->RSSetState(States::rasterCullBack);

		ShaderResourceView* hbaoSRV = hbaoEffect.process(resDepthTexture->getSRV(), gNormal->getSRV());

		ImCtx::get()->ClearRTV(originTexture->getMip(0), DirectX::Colors::Black);
		ImCtx::get()->OMSetRTV({ originTexture->getMip(0) }, nullptr);

		ImCtx::get()->PSSetSRV({ gBaseColor,gPosition,gNormal,gRoughnessMetallic,hbaoSRV }, 0);
		ImCtx::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),mainScene->lightBuffer }, 1);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(deferredFinal);

		ImCtx::get()->DrawQuad();

		/*reflectTexture->clearRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetRTV({ reflectTexture }, nullptr);
		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->PSSetSRV({ originTexture,gPosition,gNormal,depthView }, 0);
		ImCtx::get()->PSSetSampler({ States::pointClampSampler }, 0);
		ImCtx::get()->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);

		Shader::fullScreenVS->use();
		screenSpaceReflection->use();

		ImCtx::get()->DrawQuad();*/

		ShaderResourceView* bloomSRV = bloomEffect.process(originTexture);

		ImCtx::get()->OMSetBlendState(nullptr);

		ImCtx::get()->ClearDefRTV(DirectX::Colors::Black);
		ImCtx::get()->OMSetDefRTV(nullptr);
		ImCtx::get()->PSSetSampler({ States::pointClampSampler }, 0);
		ImCtx::get()->PSSetSRV({ bloomSRV }, 0);

		ImCtx::get()->BindShader(Shader::fullScreenVS);
		ImCtx::get()->BindShader(Shader::fullScreenPS);

		ImCtx::get()->DrawQuad();

		ImCtx::get()->OMSetDefRTV(resDepthTexture);
		ImCtx::get()->PSSetSRV({ skybox }, 0);
		ImCtx::get()->PSSetSampler({ States::linearClampSampler }, 0);

		ImCtx::get()->BindShader(Shader::skyboxVS);
		ImCtx::get()->BindShader(skyboxPS);

		ImCtx::get()->DrawCube();
	}


};