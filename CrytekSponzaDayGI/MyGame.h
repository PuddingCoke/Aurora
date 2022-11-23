#pragma once

#include<Aurora/Game.h>
#include<Aurora/ComputeTexture3D.h>

#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/A3D/ShadowMap.h>

#include<Aurora/PostProcessing/HBAOEffect.h>
#include<Aurora/PostProcessing/BloomEffect.h>

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	ShadowMap* shadowTexture;

	ShadowMap* shadowMap;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	RenderTexture* originTexture;

	Scene* scene;

	Buffer* lightBuffer;

	Buffer* lightProjBuffer;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* skyboxPShader;

	TextureCube* skybox;

	FPSCamera camera;

	HBAOEffect hbaoEffect;

	BloomEffect bloomEffect;

	static constexpr unsigned int shadowMapRes = 4096;

	struct Light
	{
		DirectX::XMFLOAT4 lightDir;
		DirectX::XMFLOAT4 lightColor;
	} light{};

	float exposure;

	float gamma;

	MyGame() :
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_SNORM, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		shadowMap(new ShadowMap(Graphics::getWidth(), Graphics::getHeight())),
		shadowTexture(new ShadowMap(shadowMapRes, shadowMapRes)),
		scene(Scene::create(assetPath + "/sponza.dae")),
		deferredVShader(new Shader("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(new Shader("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(new Shader("DeferredFinal.hlsl", ShaderType::Pixel)),
		skyboxPShader(new Shader("SkyboxPShader.hlsl", ShaderType::Pixel)),
		skybox(new TextureCube(assetPath + "/sky/kloppenheim_05_4k.hdr", 2048)),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f)
	{
		exposure = 1.0f;
		gamma = 1.25f;
		bloomEffect.setThreshold(0.6f);
		bloomEffect.setIntensity(0.92f);
		bloomEffect.setExposure(exposure);
		bloomEffect.setGamma(gamma);
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

			Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(deferredVShader), inputLayout.ReleaseAndGetAddressOf());
		}

		camera.registerEvent();

		{
			const float radian = Math::half_pi - 0.05f;

			light.lightDir = { 0.f,sinf(radian),cosf(radian),0.f };
			light.lightColor = { 1.f, 1.f,1.f,1.f };
			DirectX::XMStoreFloat4(&light.lightDir, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&light.lightDir)));

			lightBuffer = new Buffer(sizeof(Light), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &light);

			const float xSize = 200;

			const float ySize = 100;
			const float distance = 260.f;

			const DirectX::XMVECTOR lightCamPos = DirectX::XMVectorSet(light.lightDir.x * distance, light.lightDir.y * distance, light.lightDir.z * distance, 1.f);

			const DirectX::XMMATRIX lightProjMat = DirectX::XMMatrixOrthographicOffCenterLH(-xSize / 2.f, xSize / 2.f, -ySize / 2.f, ySize / 2.f, 1.f, 512);

			const DirectX::XMMATRIX lightViewMat = DirectX::XMMatrixLookAtLH(lightCamPos, { 0.f,0.f,0.f,1.f }, { 0.f,1.f,0.f });

			const DirectX::XMMATRIX lightMat = DirectX::XMMatrixTranspose(lightViewMat * lightProjMat);

			lightProjBuffer = new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &lightMat);
		}

		{
			RenderAPI::get()->OMSetBlendState(nullptr);
			RenderAPI::get()->IASetInputLayout(inputLayout.Get());

			RenderAPI::get()->RSSetState(States::rasterShadow);
			RenderAPI::get()->RSSetViewport(shadowMapRes, shadowMapRes);
			shadowTexture->clear();
			RenderAPI::get()->OMSetRTV({}, shadowTexture);
			RenderAPI::get()->VSSetBuffer({ lightProjBuffer }, 2);
			scene->drawGeometry(RenderAPI::shadowVS);

			RenderAPI::get()->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());
		}

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);
	}

	~MyGame()
	{
		delete shadowMap;
		delete shadowTexture;

		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete originTexture;

		delete scene;
		delete skybox;

		delete lightBuffer;
		delete lightProjBuffer;
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;

		delete skyboxPShader;
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
		
			RenderAPI::get()->RSSetState(States::rasterCullBack);

			RenderAPI::get()->GSSetShader(nullptr);
			RenderAPI::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,States::shadowSampler }, 0);

			gBaseColor->clearRTV(DirectX::Colors::Black);
			gPosition->clearRTV(DirectX::Colors::Black);
			gNormalSpecular->clearRTV(DirectX::Colors::Black);
			shadowMap->clear(D3D11_CLEAR_DEPTH);

			RenderAPI::get()->OMSetRTV({ gPosition,gNormalSpecular,gBaseColor }, shadowMap);

			scene->draw(deferredVShader, deferredPShader);

			originTexture->clearRTV(DirectX::Colors::Black);
			RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
			RenderAPI::get()->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoEffect.process(shadowMap->getSRV(), gNormalSpecular->getSRV()),shadowTexture }, 0);
			RenderAPI::get()->PSSetBuffer({ Camera::getViewBuffer(),lightBuffer,lightProjBuffer }, 1);

			RenderAPI::fullScreenVS->use();
			deferredFinal->use();

			RenderAPI::get()->DrawQuad();

			ShaderResourceView* const bloomTextureSRV = bloomEffect.process(originTexture);

			RenderAPI::get()->OMSetBlendState(nullptr);
			RenderAPI::get()->OMSetDefRTV(nullptr);
			RenderAPI::get()->PSSetSRV({ bloomTextureSRV }, 0);

			RenderAPI::fullScreenVS->use();
			RenderAPI::fullScreenPS->use();

			RenderAPI::get()->DrawQuad();

			RenderAPI::get()->OMSetDefRTV(shadowMap);
			RenderAPI::get()->PSSetSRV({ skybox }, 0);

			RenderAPI::skyboxVS->use();
			skyboxPShader->use();

			RenderAPI::get()->DrawCube();
		
	}


};