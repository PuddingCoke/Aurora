#pragma once

#include<Aurora/Game.h>
#include<Aurora/PBR/RealShading.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/A3D/DepthStencilView.h>
#include<Aurora/A3D/ShadowMap.h>
#include"Scene.h"

class MyGame :public Game
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	ComPtr<ID3D11SamplerState> wrapSampler;

	ComPtr<ID3D11SamplerState> borderSampler;

	ComPtr<ID3D11Buffer> lightMatrixBuffer;

	ComPtr<ID3D11Buffer> lightBuffer;

	struct LightInfo
	{
		DirectX::XMFLOAT4 camPos;
		DirectX::XMFLOAT4 sunDir;
	}lightInfo{};

	Scene* scene;

	Scene* curtainScene;

	DepthStencilView* depthStencilView;

	RenderTexture* shadowRTV;

	ShadowMap* shadowMap;

	Shader* skyboxPShader;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredPShaderNoTex;

	Shader* deferredFinal;

	Shader* shadowVShader;

	Shader* shadowPShader;

	Shader* debugShader;

	FPSCamera camera;

	const float theta = Math::half_pi - 0.05f;

	const float radius = 30.f;

	const float xOffset = 0.f;

	RenderTexture* positionBuffer;
	RenderTexture* baseColorBuffer;
	RenderTexture* metallicRoughnessBuffer;
	RenderTexture* normalBuffer;
	RenderTexture* lightSpacePositionBuffer;

	TextureCube* skybox;

	bool state = true;

	MyGame() :
		scene(Scene::create("NewSponza_Main_Blender_glTF.gltf")),
		curtainScene(Scene::create("NewSponza_Curtains_glTF.gltf")),
		deferredVShader(Shader::fromFile("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(Shader::fromFile("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredPShaderNoTex(Shader::fromFile("DeferredPShaderNoTex.hlsl", ShaderType::Pixel)),
		shadowVShader(Shader::fromFile("ShadowVShader.hlsl", ShaderType::Vertex)),
		shadowPShader(Shader::fromFile("ShadowPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(Shader::fromFile("DeferredFinal.hlsl", ShaderType::Pixel)),
		skyboxPShader(Shader::fromFile("SkyboxPShader.hlsl", ShaderType::Pixel)),
		shadowRTV(RenderTexture::create(4096, 4096, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black)),
		debugShader(Shader::fromFile("DebugPShader.hlsl", ShaderType::Pixel)),
		depthStencilView(DepthStencilView::create(1920, 1080, DXGI_FORMAT_D32_FLOAT)),
		shadowMap(ShadowMap::create(4096, 4096)),
		positionBuffer(RenderTexture::create(1920, 1080, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		baseColorBuffer(RenderTexture::create(1920, 1080, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		metallicRoughnessBuffer(RenderTexture::create(1920, 1080, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		normalBuffer(RenderTexture::create(1920, 1080, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		lightSpacePositionBuffer(RenderTexture::create(1920, 1080, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		skybox(TextureCube::createEquirectangularMap("kloppenheim_05_4k.hdr", 2048, DirectX::XMFLOAT3{ 0,1,0 })),
		camera({ 0,10,0 }, { 1,0,0 }, { 0,1,0 }, 10, 2)
	{
		{
			D3D11_BUFFER_DESC cbd = {};
			cbd.ByteWidth = 2 * sizeof(DirectX::XMMATRIX);
			cbd.Usage = D3D11_USAGE_IMMUTABLE;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

			struct LightMatrix
			{
				DirectX::XMMATRIX proj;
				DirectX::XMMATRIX view;
			}lightMatrix;

			const float cameraSize = 8.f;

			const float nearPlane = 1.f;

			const float farPlane = 100.f;

			lightMatrix.proj = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicLH(cameraSize * 2.f, cameraSize * 2.f, nearPlane, farPlane));
			lightMatrix.view = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH({ xOffset,radius * sinf(theta),radius * cosf(theta) }, { xOffset,0,0 }, { 0,1,0 }));

			D3D11_SUBRESOURCE_DATA data = {};
			data.pSysMem = &lightMatrix;

			Renderer::device->CreateBuffer(&cbd, &data, lightMatrixBuffer.ReleaseAndGetAddressOf());

			Renderer::context->VSSetConstantBuffers(2, 1, lightMatrixBuffer.GetAddressOf());
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
			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_BORDER;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
			sampDesc.BorderColor[0] = 0;
			sampDesc.BorderColor[1] = 0;
			sampDesc.BorderColor[2] = 0;
			sampDesc.BorderColor[3] = 1;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			Renderer::device->CreateSamplerState(&sampDesc, borderSampler.ReleaseAndGetAddressOf());
		}

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
			D3D11_BUFFER_DESC cbd = {};
			cbd.ByteWidth = sizeof(LightInfo);
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			Renderer::device->CreateBuffer(&cbd, nullptr, lightBuffer.ReleaseAndGetAddressOf());
		}

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				state = !state;
			});

		Camera::setProj(Math::pi / 3.f, Graphics::getAspectRatio(), 1.f, 10000.f);
		camera.registerEvent();
	}

	~MyGame()
	{
		delete scene;
		delete curtainScene;
		delete  depthStencilView;
		delete shadowRTV;
		delete shadowMap;
		delete  skyboxPShader;
		delete  deferredVShader;
		delete  deferredPShader;
		delete  deferredPShaderNoTex;
		delete  deferredFinal;
		delete  shadowVShader;
		delete  shadowPShader;
		delete  debugShader;
		delete positionBuffer;
		delete baseColorBuffer;
		delete metallicRoughnessBuffer;
		delete normalBuffer;
		delete lightSpacePositionBuffer;
		delete skybox;
	}

	void update(const float& dt) override
	{
		//计算阴影
		Renderer::setViewport(4096, 4096);
		shadowMap->clear();
		shadowRTV->setRTV(shadowMap->get());

		Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Renderer::setBlendState(nullptr);

		Renderer::context->IASetInputLayout(inputLayout.Get());

		shadowVShader->use();
		shadowPShader->use();

		scene->drawShadow();
		curtainScene->drawShadow();

		Renderer::setViewport(1920, 1080);

		camera.applyInput(dt);

		DirectX::XMFLOAT4 sunDir = DirectX::XMFLOAT4(0, -sinf(theta), -cosf(theta), 1.0);
		DirectX::XMFLOAT3 camPos = camera.getEye();

		lightInfo.camPos = DirectX::XMFLOAT4(camPos.x, camPos.y, camPos.z, 1.0);
		lightInfo.sunDir = sunDir;

		D3D11_MAPPED_SUBRESOURCE mappedData;
		Renderer::context->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
		memcpy(mappedData.pData, &lightInfo, sizeof(LightInfo));
		Renderer::context->Unmap(lightBuffer.Get(), 0);
	}

	void render()
	{
		positionBuffer->clearRTV(DirectX::Colors::Black);
		baseColorBuffer->clearRTV(DirectX::Colors::Black);
		metallicRoughnessBuffer->clearRTV(DirectX::Colors::Black);
		normalBuffer->clearRTV(DirectX::Colors::Black);
		lightSpacePositionBuffer->clearRTV(DirectX::Colors::Black);

		depthStencilView->clear(D3D11_CLEAR_DEPTH);

		Renderer::setSampler(0, wrapSampler.Get());
		Renderer::setSampler(1, StateCommon::defSamplerState.Get());
		Renderer::setSampler(2, borderSampler.Get());

		baseColorBuffer->setRTV(depthStencilView->get());

		TextureCube::shader->use();
		skyboxPShader->use();

		skybox->setSRV(0);

		Renderer::draw(36, 0);

		RenderTexture::setRTVs({ positionBuffer,baseColorBuffer,metallicRoughnessBuffer,normalBuffer,lightSpacePositionBuffer }, depthStencilView->get());

		Renderer::context->IASetInputLayout(inputLayout.Get());

		deferredVShader->use();
		scene->draw(deferredPShader, deferredPShaderNoTex);
		curtainScene->draw(deferredPShader, deferredPShaderNoTex);

		Renderer::clearDefRTV(DirectX::Colors::Black);
		Renderer::setDefRTV();

		Shader::displayVShader->use();

		if (state)
		{
			positionBuffer->getTexture()->setSRV(0);
			baseColorBuffer->getTexture()->setSRV(1);
			metallicRoughnessBuffer->getTexture()->setSRV(2);
			normalBuffer->getTexture()->setSRV(3);
			lightSpacePositionBuffer->getTexture()->setSRV(4);
			shadowMap->setSRV(5);
			Renderer::context->PSSetConstantBuffers(3, 1, lightBuffer.GetAddressOf());
			deferredFinal->use();
		}
		else
		{
			shadowMap->setSRV(0);
			debugShader->use();
		}


		Renderer::draw(3, 0);

		ID3D11ShaderResourceView* nullView[8] = { nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr,nullptr };
		Renderer::context->PSSetShaderResources(0, 8, nullView);
	}


};