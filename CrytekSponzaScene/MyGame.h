#pragma once

#include<Aurora/Game.h>
#include<Aurora/Texture2D.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/DepthStencilView.h>
#include<Aurora/TextureCube.h>
#include<Aurora/A3D/FPSCamera.h>

#include"Scene.h"

class MyGame :public Game
{
public:

	RenderTexture* positionDepth;

	RenderTexture* normalSpecular;

	RenderTexture* ssaoTexture;

	RenderTexture* ssaoBlured;

	RenderTexture* albedo;

	TextureCube* skybox;

	Texture2D* randomNormal;

	DepthStencilView* depthStencilView;

	ComPtr<ID3D11InputLayout> inputLayout;

	Shader* skyboxPShader;

	Shader* ssaoShader;

	Shader* ssaoBlur;

	Shader* deferredFinal;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Scene* scene;

	ComPtr<ID3D11SamplerState> wrapSampler;

	ComPtr<ID3D11Buffer> lightBuffer;

	ComPtr<ID3D11Buffer> ssaoBuffer;

	FPSCamera camera;

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
	}lightInfo{}, viewSpaceLight{};

	struct SSAOParams
	{
		float g_sample_rad = 1.00f;
		float g_intensity = 4.0f;
		float g_scale = 0.53f;
		float g_bias = 0.025f;
	} ssaoParams;

	MyGame() :
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.0f,0.0f }, { 0.f,1.f,0.f }, 100.f, 5.f),
		deferredVShader(Shader::fromFile("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(Shader::fromFile("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(Shader::fromFile("DeferredFinal.hlsl", ShaderType::Pixel)),
		ssaoBlur(Shader::fromFile("SSAOBlur.hlsl", ShaderType::Pixel)),
		skyboxPShader(Shader::fromFile("SkyboxPShader.hlsl", ShaderType::Pixel)),
		positionDepth(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		normalSpecular(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		albedo(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black, false)),
		ssaoTexture(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_FLOAT, DirectX::Colors::Black, false)),
		ssaoBlured(RenderTexture::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32_FLOAT, DirectX::Colors::Black, false)),
		depthStencilView(DepthStencilView::create(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_D32_FLOAT, false)),
		randomNormal(Texture2D::create("RandomNormal.png")),
		ssaoShader(Shader::fromFile("SSAOShader.hlsl", ShaderType::Pixel)),
		scene(Scene::create(assetPath + "/sponza.dae")),
		skybox(TextureCube::create({ assetPath + "/sky/SkyEarlyDusk_Right.png",assetPath + "/sky/SkyEarlyDusk_Left.png",assetPath + "/sky/SkyEarlyDusk_Top.png",assetPath + "/sky/SkyEarlyDusk_Bottom.png",assetPath + "/sky/SkyEarlyDusk_Front.png",assetPath + "/sky/SkyEarlyDusk_Back.png" }))
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
			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			Renderer::device->CreateSamplerState(&sampDesc, wrapSampler.ReleaseAndGetAddressOf());
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
				setLight(&viewSpaceLight.lights[i], { (float)(i - 2.5f) * 50.0f, 10.0f, 0.0f }, lightColors[i], 120.0f);
			}

			setLight(&viewSpaceLight.lights[5], { -48.75f, 16.0f, -17.8f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&viewSpaceLight.lights[6], { -48.75f, 16.0f,  18.4f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&viewSpaceLight.lights[7], { 62.0f, 16.0f, -17.8f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&viewSpaceLight.lights[8], { 62.0f, 16.0f,  18.4f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&viewSpaceLight.lights[9], { 120.0f, 20.0f, -43.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&viewSpaceLight.lights[10], { 120.0f, 20.0f, 41.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&viewSpaceLight.lights[11], { -110.0f, 20.0f, -43.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&viewSpaceLight.lights[12], { -110.0f, 20.0f, 41.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&viewSpaceLight.lights[13], { -122.0f, 18.0f, -3.2f }, { 1.0f, 0.3f, 0.3f }, 25.0f);
			setLight(&viewSpaceLight.lights[14], { -122.0f, 18.0f,  3.2f }, { 0.3f, 1.0f, 0.3f }, 25.0f);
			setLight(&viewSpaceLight.lights[15], { 135.0f, 18.0f, -3.2f }, { 0.3f, 0.3f, 1.0f }, 25.0f);
			setLight(&viewSpaceLight.lights[16], { 135.0f, 18.0f,  3.2f }, { 1.0f, 1.0f, 0.3f }, 25.0f);
		}

		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(LightInfo);
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			Renderer::device->CreateBuffer(&bd, nullptr, lightBuffer.ReleaseAndGetAddressOf());
		}

		{
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(SSAOParams);
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			D3D11_SUBRESOURCE_DATA subresource = {};
			subresource.pSysMem = &ssaoParams;

			Renderer::device->CreateBuffer(&bd, &subresource, ssaoBuffer.ReleaseAndGetAddressOf());
		}

		camera.registerEvent();

		Camera::setProj(DirectX::XMMatrixPerspectiveFovLH(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f));
	}

	~MyGame()
	{
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete positionDepth;
		delete normalSpecular;
		delete albedo;
		delete depthStencilView;
		delete scene;
		delete randomNormal;
		delete ssaoShader;
		delete ssaoTexture;
		delete ssaoBlured;
		delete ssaoBlur;
		delete skybox;
		delete skyboxPShader;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);

		D3D11_MAPPED_SUBRESOURCE mappedData;
		Renderer::context->Map(ssaoBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &ssaoParams, sizeof(SSAOParams));
		Renderer::context->Unmap(ssaoBuffer.Get(), 0);

		lightInfo.lights[0].position = DirectX::XMFLOAT4(-sinf(360.0f * Graphics::getSTime() / 2.f * Math::degToRad) * 120.0f, 3.5f, cosf(360.0f * Graphics::getSTime() * 8.0f * Math::degToRad / 2.f) * 10.0f, 1.f);

		for (unsigned int i = 0; i < 17; i++)
		{
			DirectX::XMFLOAT3 lightInViewSpace = Camera::toViewSpace(DirectX::XMFLOAT3(lightInfo.lights[i].position.x, lightInfo.lights[i].position.y, lightInfo.lights[i].position.z));
			viewSpaceLight.lights[i].position = DirectX::XMFLOAT4(lightInViewSpace.x, lightInViewSpace.y, lightInViewSpace.z, 1.f);
		}

		Renderer::context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &viewSpaceLight, sizeof(LightInfo));
		Renderer::context->Unmap(lightBuffer.Get(), 0);

		Renderer::context->PSSetConstantBuffers(3, 1, lightBuffer.GetAddressOf());
	}

	void render()
	{
		Renderer::setBlendState(nullptr);

		albedo->clearRTV(DirectX::Colors::Black);
		albedo->setRTV();

		Renderer::context->PSSetSamplers(0, 1, wrapSampler.GetAddressOf());
		Renderer::context->PSSetSamplers(1, 1, StateCommon::defSamplerState.GetAddressOf());

		skybox->setSRV(0);

		TextureCube::shader->use();
		skyboxPShader->use();

		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Renderer::context->Draw(36, 0);

		positionDepth->clearRTV(DirectX::Colors::Black);
		normalSpecular->clearRTV(DirectX::Colors::Black);

		depthStencilView->clear(D3D11_CLEAR_DEPTH);
		RenderTexture::setRTVs({ positionDepth,normalSpecular,albedo }, depthStencilView->get());

		Renderer::context->IASetInputLayout(inputLayout.Get());

		deferredVShader->use();
		deferredPShader->use();

		scene->draw();

		ssaoTexture->clearRTV(DirectX::Colors::Black);
		ssaoTexture->setRTV();

		positionDepth->getTexture()->setSRV(0);
		normalSpecular->getTexture()->setSRV(1);
		randomNormal->setSRV(2);

		Renderer::context->PSSetConstantBuffers(0, 1, ssaoBuffer.GetAddressOf());

		Shader::displayVShader->use();
		ssaoShader->use();

		Renderer::context->Draw(3, 0);

		ssaoBlured->clearRTV(DirectX::Colors::Black);
		ssaoBlured->setRTV();

		ssaoTexture->getTexture()->setSRV(0);

		Shader::displayVShader->use();
		ssaoBlur->use();

		Renderer::context->Draw(3, 0);

		Renderer::setDefRTV();

		positionDepth->getTexture()->setSRV(0);
		normalSpecular->getTexture()->setSRV(1);
		albedo->getTexture()->setSRV(2);
		ssaoBlured->getTexture()->setSRV(3);

		Shader::displayVShader->use();
		deferredFinal->use();

		Renderer::context->Draw(3, 0);

		ID3D11ShaderResourceView* nullView[4] = { nullptr,nullptr,nullptr,nullptr };

		Renderer::context->PSSetShaderResources(0, 4, nullView);
	}


};