﻿#pragma once

#include<Aurora/Game.h>
#include<Aurora/Texture2D.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/A3D/DepthStencilView.h>
#include<Aurora/A3D/TextureCube.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/A3D/ShadowMap.h>
#include<Aurora/A3D/CascadedShadowMap.h>
#include<Aurora/PostProcessing/HBAOEffect.h>

#include"Scene.h"

class MyGame :public Game
{
public:

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	TextureCube* skybox;

	ShadowMap* shadowMap;

	ShadowMap* shadowMap2;

	ComPtr<ID3D11InputLayout> inputLayout;

	Shader* skyboxPShader;

	Shader* deferredFinal;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* debugPShader;

	Scene* scene;

	ComPtr<ID3D11Buffer> lightBuffer;

	FPSCamera camera;

	HBAOEffect hbaoEffect;

	CascadedShadowMap csm;

	bool depthMode = false;

	struct LightInfo
	{
		struct Light
		{
			DirectX::XMFLOAT4 position;
			DirectX::XMFLOAT4 color;
			float radius;
			float quadraticFalloff;
			float linearFalloff;
			float v0;
		}lights[17]{};
	}lightInfo{};

	MyGame() :
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.0f,0.0f }, { 0.f,1.f,0.f }, 100.f, 3.f),
		deferredVShader(Shader::fromFile("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(Shader::fromFile("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(Shader::fromFile("DeferredFinal.hlsl", ShaderType::Pixel)),
		skyboxPShader(Shader::fromFile("SkyboxPShader.hlsl", ShaderType::Pixel)),
		debugPShader(Shader::fromFile("DebugPShader.hlsl", ShaderType::Pixel)),
		gPosition(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gBaseColor(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		shadowMap(ShadowMap::create(Graphics::getWidth(), Graphics::getHeight())),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		scene(Scene::create(assetPath + "/sponza.dae")),
		skybox(TextureCube::create({ assetPath + "/sky/SkyEarlyDusk_Right.png",assetPath + "/sky/SkyEarlyDusk_Left.png",assetPath + "/sky/SkyEarlyDusk_Top.png",assetPath + "/sky/SkyEarlyDusk_Bottom.png",assetPath + "/sky/SkyEarlyDusk_Front.png",assetPath + "/sky/SkyEarlyDusk_Back.png" })),
		csm(Graphics::getWidth(), Graphics::getHeight(), { 0,100,20 }, { 0,0,0 }),
		shadowMap2(ShadowMap::create(2048, 2048))
	{

		{
			D3D11_INPUT_ELEMENT_DESC layout[5] =
			{
				{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{"TANGENT", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};

			Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), deferredVShader->shaderBlob->GetBufferPointer(), deferredVShader->shaderBlob->GetBufferSize(), inputLayout.ReleaseAndGetAddressOf());
		}

		{
			auto setLight = [](LightInfo::Light* light, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& color, const float& radius)
			{
				light->position = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1.f);
				light->color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.f);
				light->radius = radius;
			};

			DirectX::XMFLOAT3 lightColors[5];
			lightColors[0] = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
			lightColors[1] = DirectX::XMFLOAT3(1.0f, 0.7f, 0.7f);
			lightColors[2] = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
			lightColors[3] = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);
			lightColors[4] = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);

			for (int i = 0; i < 5; i++)
			{
				setLight(&lightInfo.lights[i], { (float)(i - 2.5f) * 50.0f, 10.0f, 0.0f }, lightColors[i], 120.0f);
			}

			setLight(&lightInfo.lights[5], { -48.75f, 16.0f, -17.8f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[6], { -48.75f, 16.0f,  18.4f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[7], { 62.0f, 16.0f, -17.8f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[8], { 62.0f, 16.0f,  18.4f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[9], { 120.0f, 20.0f, -43.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[10], { 120.0f, 20.0f, 41.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[11], { -110.0f, 20.0f, -43.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[12], { -110.0f, 20.0f, 41.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[13], { -122.0f, 18.0f, -3.2f }, { 1.0f, 0.3f, 0.3f }, 25.0f);
			setLight(&lightInfo.lights[14], { -122.0f, 18.0f,  3.2f }, { 0.3f, 1.0f, 0.3f }, 25.0f);
			setLight(&lightInfo.lights[15], { 135.0f, 18.0f, -3.2f }, { 0.3f, 0.3f, 1.0f }, 25.0f);
			setLight(&lightInfo.lights[16], { 135.0f, 18.0f,  3.2f }, { 1.0f, 1.0f, 0.3f }, 25.0f);
		}

		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(LightInfo);
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			Renderer::device->CreateBuffer(&bd, nullptr, lightBuffer.ReleaseAndGetAddressOf());
		}

		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 1000.f);

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				depthMode = !depthMode;
			});

	}

	~MyGame()
	{
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete debugPShader;
		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete scene;
		delete shadowMap;
		delete shadowMap2;
		delete skybox;
		delete skyboxPShader;
	}

	void update(const float& dt) override
	{
		Renderer::context->IASetInputLayout(inputLayout.Get());
		Renderer::setBlendState(nullptr);

		camera.applyInput(dt);

		lightInfo.lights[0].position = DirectX::XMFLOAT4(-sinf(360.0f * Graphics::getSTime() / 2.f * Math::degToRad) * 120.0f, 3.5f, cosf(360.0f * Graphics::getSTime() * 8.0f * Math::degToRad / 2.f) * 10.0f, 1.f);

		D3D11_MAPPED_SUBRESOURCE mappedData = {};

		Renderer::context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &lightInfo, sizeof(LightInfo));
		Renderer::context->Unmap(lightBuffer.Get(), 0);

		csm.renderShaodwMap(shadowMap2, []() {});

		Renderer::setViewport(2048, 2048);

		shadowMap2->clear();
		Renderer::context->PSSetShader(nullptr, nullptr, 0);
		Renderer::context->OMSetRenderTargets(0, nullptr, shadowMap2->get());

		scene->draw(ShadowMap::shadowVShader, nullptr);

		Renderer::setViewport(Graphics::getWidth(), Graphics::getHeight());
	}

	void render()
	{
		Renderer::setSampler(0, StateCommon::linearWrapSampler.Get());
		Renderer::setSampler(1, StateCommon::linearClampSampler.Get());

		gBaseColor->clearRTV(DirectX::Colors::Black);
		gPosition->clearRTV(DirectX::Colors::Black);
		gNormalSpecular->clearRTV(DirectX::Colors::Black);
		shadowMap->clear(D3D11_CLEAR_DEPTH);

		RenderTexture::setRTVs({ gPosition,gNormalSpecular,gBaseColor }, shadowMap->get());

		scene->draw(deferredVShader, deferredPShader);

		Texture2D* const ssaoTexture = hbaoEffect.process(shadowMap->getSRV(), gNormalSpecular->getTexture()->getSRV());

		Renderer::setDefRTV();
		
		if (depthMode)
		{
			Shader::displayVShader->use();
			debugPShader->use();

			shadowMap2->setSRV(0);

			Renderer::drawQuad();
		}
		else
		{
			gPosition->getTexture()->setSRV(0);
			gNormalSpecular->getTexture()->setSRV(1);
			gBaseColor->getTexture()->setSRV(2);
			ssaoTexture->setSRV(3);

			ID3D11Buffer* buffers[2] = { Camera::getViewBuffer(),lightBuffer.Get() };

			Renderer::context->PSSetConstantBuffers(1, 2, buffers);

			Shader::displayVShader->use();
			deferredFinal->use();

			Renderer::context->Draw(3, 0);

			Renderer::setDefRTV(shadowMap->get());

			skybox->setSRV(0);

			TextureCube::shader->use();
			skyboxPShader->use();

			Renderer::drawCube();
		}

		ID3D11ShaderResourceView* nullView[4] = { nullptr,nullptr,nullptr,nullptr };

		Renderer::context->PSSetShaderResources(0, 4, nullView);
	}


};