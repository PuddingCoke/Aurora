#pragma once

#include<Aurora/Game.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/A3D/ShadowMap.h>
#include<Aurora/A3D/TextureCube.h>
#include<Aurora/PostProcessing/BloomEffect.h>
#include<Aurora/PostProcessing/HBAOEffect.h>

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	ComPtr<ID3D11SamplerState> borderSampler;

	FPSCamera camera;

	ShadowMap* depthView;

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
		depthView(new ShadowMap(Graphics::getWidth(), Graphics::getHeight())),
		mainScene(new Scene("E:/Assets/SuperSponza/NewSponza_Main_glTF_002.gltf")),
		//curtainScene(new Scene("E:/Assets/SuperSponza/NewSponza_Curtains_glTF.gltf")),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM)),
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT)),
		gNormal(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT)),
		gRoughnessMetallic(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		reflectTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
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

			Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(deferredVS), inputLayout.ReleaseAndGetAddressOf());
		}

		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 0.1f, 35.f);
	}

	~MyGame()
	{
		delete mainScene;
		//delete curtainScene;

		delete depthView;

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
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());
		RenderAPI::get()->PSSetSampler({ States::anisotropicWrapSampler,States::linearClampSampler }, 0);

		depthView->clear();
		gBaseColor->clearRTV(DirectX::Colors::Black);
		gPosition->clearRTV(DirectX::Colors::Black);
		gNormal->clearRTV(DirectX::Colors::Black);
		gRoughnessMetallic->clearRTV(DirectX::Colors::Black);

		RenderAPI::get()->OMSetRTV({ gBaseColor,gPosition,gNormal,gRoughnessMetallic }, depthView);

		mainScene->draw(deferredVS, deferredPSHasTex, deferredPSNoTex, deferredPSTrans);

		//RenderAPI::get()->RSSetState(States::rasterCullNone);
		//curtainScene->draw(deferredVS, deferredPSHasTex, deferredPSNoTex, deferredPSTrans);
		//RenderAPI::get()->RSSetState(States::rasterCullBack);

		ShaderResourceView* hbaoSRV = hbaoEffect.process(depthView->getSRV(), gNormal->getSRV());

		originTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);

		RenderAPI::get()->PSSetSRV({ gBaseColor,gPosition,gNormal,gRoughnessMetallic,hbaoSRV }, 0);
		RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),mainScene->lightBuffer }, 1);

		RenderAPI::fullScreenVS->use();
		deferredFinal->use();

		RenderAPI::get()->DrawQuad();

		/*reflectTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ reflectTexture }, nullptr);
		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->PSSetSRV({ originTexture,gPosition,gNormal,depthView }, 0);
		RenderAPI::get()->PSSetSampler({ States::pointClampSampler }, 0);
		RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);

		RenderAPI::fullScreenVS->use();
		screenSpaceReflection->use();

		RenderAPI::get()->DrawQuad();*/

		ShaderResourceView* bloomSRV = bloomEffect.process(originTexture);

		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetSampler({ States::pointClampSampler }, 0);
		RenderAPI::get()->PSSetSRV({ bloomSRV }, 0);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();

		RenderAPI::get()->OMSetDefRTV(depthView);
		RenderAPI::get()->PSSetSRV({ skybox }, 0);
		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);

		RenderAPI::skyboxVS->use();
		skyboxPS->use();

		RenderAPI::get()->DrawCube();
	}


};