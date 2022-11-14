#pragma once

#include<Aurora/Game.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/A3D/ShadowMap.h>
#include<Aurora/PostProcessing/BloomEffect.h>

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	ShadowMap* depthView;

	FPSCamera camera;

	Scene* mainScene;

	Scene* curtainScene;

	RenderTexture* gBaseColor;

	RenderTexture* gPosition;

	RenderTexture* gNormal;

	RenderTexture* gRoughnessMetallic;

	RenderTexture* originTexture;

	Shader* deferredVS;

	//有贴图
	Shader* deferredPSHasTex;

	//无贴图
	Shader* deferredPSNoTex;

	//透明材质
	Shader* deferredPSTrans;

	Shader* debugPS;

	Shader* deferredFinal;

	BloomEffect bloomEffect;

	float exposure;

	float gamma;

	MyGame() :
		camera({ 0,10,0 }, { 1,0,0 }, { 0,1,0 }, 10, 0.f),
		depthView(new ShadowMap(Graphics::getWidth(), Graphics::getHeight())),
		mainScene(new Scene("D:\\Assets\\SuperSponza\\NewSponza_Main_glTF_002.gltf")),
		curtainScene(new Scene("D:\\Assets\\SuperSponza\\NewSponza_Curtains_glTF.gltf")),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM)),
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT)),
		gNormal(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT)),
		gRoughnessMetallic(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		deferredVS(new Shader("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPSHasTex(new Shader("DeferredPSHasTex.hlsl", ShaderType::Pixel)),
		deferredPSNoTex(new Shader("DeferredPSNoTex.hlsl", ShaderType::Pixel)),
		deferredPSTrans(new Shader("DeferredPSTrans.hlsl", ShaderType::Pixel)),
		debugPS(new Shader("DebugPS.hlsl", ShaderType::Pixel)),
		deferredFinal(new Shader("DeferredFinal.hlsl", ShaderType::Pixel)),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight())
	{
		exposure = bloomEffect.getExposure();
		gamma = bloomEffect.getGamma();
		bloomEffect.setThreshold(2.f);
		bloomEffect.setIntensity(0.8f);
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

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);
	}

	~MyGame()
	{
		delete mainScene;
		delete curtainScene;

		delete depthView;

		delete gBaseColor;
		delete gPosition;
		delete gNormal;
		delete gRoughnessMetallic;
		delete originTexture;

		delete deferredVS;
		delete deferredPSHasTex;
		delete deferredPSNoTex;
		delete deferredPSTrans;
		delete debugPS;
		delete deferredFinal;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);

		if (Keyboard::getKeyDown(Keyboard::Z))
		{
			exposure += 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::X))
		{
			exposure -= 0.01f;
			bloomEffect.setExposure(exposure);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::N))
		{
			gamma += 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
		}
		else if (Keyboard::getKeyDown(Keyboard::M))
		{
			gamma -= 0.01f;
			bloomEffect.setGamma(gamma);
			bloomEffect.applyChange();
		}
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

		RenderAPI::get()->RSSetState(States::rasterCullNone);

		curtainScene->draw(deferredVS, deferredPSHasTex, deferredPSNoTex, deferredPSTrans);

		RenderAPI::get()->RSSetState(States::rasterCullBack);

		originTexture->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);

		RenderAPI::get()->PSSetSRV({ gBaseColor,gPosition,gNormal,gRoughnessMetallic }, 0);
		RenderAPI::get()->PSSetBuffer({ Camera::getViewBuffer(),mainScene->lightBuffer }, 1);

		RenderAPI::fullScreenVS->use();
		deferredFinal->use();

		RenderAPI::get()->DrawQuad();

		ShaderResourceView* bloomSRV = bloomEffect.process(originTexture);

		RenderAPI::get()->OMSetBlendState(nullptr);

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);

		RenderAPI::get()->PSSetSampler({ States::linearClampSampler }, 0);
		RenderAPI::get()->PSSetSRV({ bloomSRV }, 0);

		RenderAPI::fullScreenVS->use();
		RenderAPI::fullScreenPS->use();

		RenderAPI::get()->DrawQuad();

	}


};