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
#include<Aurora/PostProcessing/BloomEffect.h>
#include"Scene.h"

class MyGame :public Game
{
public:

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	RenderTexture* originTexture;

	TextureCube* skybox;

	ShadowMap* depthView;

	ComPtr<ID3D11InputLayout> inputLayout;

	Shader* skyboxPShader;

	Shader* deferredFinal;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Scene* scene;

	ComPtr<ID3D11Buffer> lightBuffer;

	FPSCamera camera;

	HBAOEffect hbaoEffect;

	BloomEffect bloomEffect;

	CascadedShadowMap csm;

	float exposure;

	float gamma;

	float angle = Math::half_pi;

	static constexpr float lightRadius = 600.f;

	static constexpr float lightIntensity = 1.0f;

	struct Light
	{
		DirectX::XMFLOAT4 lightDir;
		DirectX::XMFLOAT4 lightColor = { lightIntensity,lightIntensity,lightIntensity,1 };
	}light{};

	void setLight()
	{
		const DirectX::XMFLOAT3 lightPos = { 0,lightRadius * sinf(angle),lightRadius * cosf(angle) };

		csm.setLightPos(lightPos);

		DirectX::XMFLOAT3 lightPosNorm;

		DirectX::XMStoreFloat3(&lightPosNorm, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&lightPos)));

		light.lightDir = { lightPosNorm.x,lightPosNorm.y, lightPosNorm.z,1.f };

		D3D11_MAPPED_SUBRESOURCE mappedData = {};
		Renderer::context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &light, sizeof(Light));
		Renderer::context->Unmap(lightBuffer.Get(), 0);
	}

	MyGame() :
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f, 3.f),
		deferredVShader(Shader::fromFile("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(Shader::fromFile("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(Shader::fromFile("DeferredFinal.hlsl", ShaderType::Pixel)),
		skyboxPShader(Shader::fromFile("SkyboxPShader.hlsl", ShaderType::Pixel)),
		gPosition(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gBaseColor(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		originTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		depthView(ShadowMap::create(Graphics::getWidth(), Graphics::getHeight())),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		scene(Scene::create(assetPath + "/sponza.dae")),
		csm(Graphics::getWidth(), Graphics::getHeight(), { 0,600,100 }, { 0,0,0 }),
		skybox(TextureCube::createEquirectangularMap(assetPath + "/sky/kloppenheim_05_4k.hdr", 2048, { 0,1,0 }))
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
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(light);
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			Renderer::device->CreateBuffer(&bd, nullptr, lightBuffer.ReleaseAndGetAddressOf());
		}

		camera.registerEvent();

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 600.f);

		/*Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				std::cout << gamma << " " << exposure << "\n";
			});*/

		gamma = 1.20f;
		exposure = 0.90f;

		bloomEffect.setGamma(gamma);
		bloomEffect.setExposure(exposure);
		bloomEffect.setThreshold(0.7f);
		bloomEffect.setIntensity(0.8f);
		bloomEffect.applyChange();

		setLight();
	}

	~MyGame()
	{
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete originTexture;
		delete scene;
		delete depthView;
		delete skybox;
		delete skyboxPShader;
	}

	void update(const float& dt) override
	{
		/*if (Keyboard::getKeyDown(Keyboard::Z))
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
		}*/

		if (Keyboard::getKeyDown(Keyboard::Q))
		{
			angle += 0.01f;
			setLight();
		}

		if (Keyboard::getKeyDown(Keyboard::E))
		{
			angle -= 0.01f;
			setLight();
		}

		camera.applyInput(dt);

		csm.updateMatrices();
	}

	void render()
	{
		Renderer::context->IASetInputLayout(inputLayout.Get());

		Renderer::setBlendState(nullptr);

		Renderer::context->RSSetState(States::rasterCullBack.Get());

		Renderer::setSampler(0, States::linearWrapSampler.Get());
		Renderer::setSampler(1, States::linearClampSampler.Get());

		Renderer::setViewport(Graphics::getWidth(), Graphics::getHeight());

		gBaseColor->clearRTV(DirectX::Colors::Black);
		gPosition->clearRTV(DirectX::Colors::Black);
		gNormalSpecular->clearRTV(DirectX::Colors::Black);
		depthView->clear();

		RenderTexture::setRTVs({ gPosition,gNormalSpecular,gBaseColor }, depthView->get());

		scene->draw(deferredVShader, deferredPShader);

		Texture2D* const hbaoTexture = hbaoEffect.process(depthView->getSRV(), gNormalSpecular->getTexture()->getSRV());

		Renderer::context->RSSetState(States::rasterCullNone.Get());

		Renderer::context->OMSetRenderTargets(0, nullptr, nullptr);

		csm.renderShaodwMap(depthView, [this]() {
			scene->drawGeometry(ShadowMap::shadowVShader);
			});

		scene->drawRayTrace(ShadowMap::shadowVShader, &csm);

		csm.renderFrustumTrace([this]()
			{
				scene->drawGeometry(ShadowMap::shadowVShader);
			});

		Renderer::context->RSSetState(States::rasterCullBack.Get());

		ID3D11ShaderResourceView* shadowSRV = csm.getShadowBuffer();

		originTexture->setRTV();

		gPosition->getTexture()->setSRV(0);
		gNormalSpecular->getTexture()->setSRV(1);
		gBaseColor->getTexture()->setSRV(2);
		hbaoTexture->setSRV(3);
		Renderer::context->PSSetShaderResources(4, 1, &shadowSRV);

		ID3D11Buffer* buffers[2] = { Camera::getViewBuffer(),lightBuffer.Get() };

		Renderer::context->PSSetConstantBuffers(1, 2, buffers);

		Shader::displayVShader->use();
		deferredFinal->use();

		Renderer::context->Draw(3, 0);

		Texture2D* const bloomTexture = bloomEffect.process(originTexture->getTexture());

		Renderer::setBlendState(nullptr);

		Renderer::setDefRTV();

		bloomTexture->setSRV(0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		Renderer::drawQuad();

		Renderer::setDefRTV(depthView->get());

		skybox->setSRV(0);

		TextureCube::shader->use();
		skyboxPShader->use();

		Renderer::drawCube();

		ID3D11ShaderResourceView* nullView[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };

		Renderer::context->PSSetShaderResources(0, 5, nullView);
	}


};