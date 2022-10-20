#pragma once

#include<Aurora/Game.h>
#include<Aurora/ResourceTexture.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/DX/Resource/Buffer.h>
#include<Aurora/DX/View/DepthStencilView.h>
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

	Buffer* lightBuffer;

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
		const DirectX::XMFLOAT3 lightPos = { 50,lightRadius * sinf(angle),lightRadius * cosf(angle) };

		csm.setLightPos(lightPos);

		DirectX::XMFLOAT3 lightPosNorm;

		DirectX::XMStoreFloat3(&lightPosNorm, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&lightPos)));

		light.lightDir = { lightPosNorm.x,lightPosNorm.y, lightPosNorm.z,1.f };

		D3D11_MAPPED_SUBRESOURCE mappedData = lightBuffer->map(0);
		memcpy(mappedData.pData, &light, sizeof(Light));
		lightBuffer->unmap(0);
	}

	MyGame() :
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f, 3.f),
		deferredVShader(Shader::fromFile("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(Shader::fromFile("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(Shader::fromFile("DeferredFinal.hlsl", ShaderType::Pixel)),
		skyboxPShader(Shader::fromFile("SkyboxPShader.hlsl", ShaderType::Pixel)),
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		depthView(ShadowMap::create(Graphics::getWidth(), Graphics::getHeight())),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		scene(Scene::create(assetPath + "/sponza.dae")),
		csm(Graphics::getWidth(), Graphics::getHeight(), { 0,600,100 }, { 0,0,0 }),
		skybox(TextureCube::createEquirectangularMap(assetPath + "/sky/kloppenheim_05_4k.hdr", 4096, { 0,1,0 })),
		lightBuffer(new Buffer(sizeof(light), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE))
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

			Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(deferredVShader), inputLayout.ReleaseAndGetAddressOf());
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
		delete lightBuffer;
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
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());
		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->RSSetState(States::get()->rasterCullBack.Get());

		ID3D11SamplerState* samplers[] = { States::get()->linearWrapSampler.Get(),States::get()->linearClampSampler.Get() };

		RenderAPI::get()->PSSetSampler(samplers, 0, 2);
		RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		gBaseColor->clearRTV(DirectX::Colors::Black);
		gPosition->clearRTV(DirectX::Colors::Black);
		gNormalSpecular->clearRTV(DirectX::Colors::Black);
		depthView->clear();

		RenderAPI::get()->OMSetRTV({ gPosition,gNormalSpecular,gBaseColor }, depthView->get());

		scene->draw(deferredVShader, deferredPShader);

		ShaderResourceView* const hbaoTexture = hbaoEffect.process(depthView->getSRV(), gNormalSpecular->getSRV());

		RenderAPI::get()->RSSetState(States::get()->rasterCullNone.Get());
		RenderAPI::get()->UnbindRTV();

		csm.renderShaodwMap(depthView, [this]() {
			scene->drawGeometry(ShadowMap::shadowVShader);
			});

		scene->drawRayTrace(ShadowMap::shadowVShader, &csm);

		csm.renderFrustumTrace([this]()
			{
				scene->drawGeometry(ShadowMap::shadowVShader);
			});

		RenderAPI::get()->RSSetState(States::get()->rasterCullBack.Get());

		ShaderResourceView* const shadowSRV = csm.getShadowBuffer();

		RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
		RenderAPI::get()->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoTexture,shadowSRV }, 0);
		RenderAPI::get()->PSSetBuffer({ Camera::getViewBuffer(),lightBuffer }, 1);

		Shader::displayVShader->use();
		deferredFinal->use();

		RenderAPI::get()->DrawQuad();

		ShaderResourceView* const bloomTexture = bloomEffect.process(originTexture);

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetSRV({ bloomTexture }, 0);

		Shader::displayVShader->use();
		Shader::displayPShader->use();

		RenderAPI::get()->DrawQuad();

		RenderAPI::get()->OMSetDefRTV(depthView->get());
		RenderAPI::get()->PSSetSRV({ skybox }, 0);

		TextureCube::shader->use();
		skyboxPShader->use();

		RenderAPI::get()->DrawCube();
	}


};