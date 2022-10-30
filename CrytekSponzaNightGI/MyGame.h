#pragma once

#include<Aurora/Game.h>
#include<Aurora/ComputeTexture3D.h>
#include<Aurora/A3D/FPSCamera.h>
#include<Aurora/A3D/ShadowMap.h>
#include<Aurora/PostProcessing/HBAOEffect.h>

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	ShadowMap* shadowMap;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	ComputeTexture3D* voxelTextureUint;

	ComputeTexture3D* voxelTextureColor;

	Scene* scene;

	Buffer* voxelParamBuffer;

	Buffer* voxelProjBuffer;

	Buffer* lightBuffer;

	Shader* voxelVShader;

	Shader* voxelGShader;

	Shader* voxelPShader;

	Shader* voxelConvert;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* visualVShader;

	Shader* visualGShader;

	Shader* visualPShader;

	FPSCamera camera;

	HBAOEffect hbaoEffect;

	struct VoxelParam
	{
		unsigned int voxelGridRes;
		float voxelGridLength;
		DirectX::XMFLOAT2 v0;
	} voxelParam{};

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
		}lights[10]{};
	}lightInfo{};

	MyGame() :
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black)),
		voxelProjBuffer(new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
		shadowMap(ShadowMap::create(Graphics::getWidth(), Graphics::getHeight())),
		scene(Scene::create(assetPath + "/sponza.dae")),
		voxelVShader(Shader::fromFile("VoxelizationVShader.hlsl", ShaderType::Vertex)),
		voxelGShader(Shader::fromFile("VoxelizationGShader.hlsl", ShaderType::Geometry)),
		voxelPShader(Shader::fromFile("VoxelizationPShader.hlsl", ShaderType::Pixel)),
		voxelConvert(Shader::fromFile("VoxelConvert.hlsl", ShaderType::Compute)),
		deferredVShader(Shader::fromFile("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(Shader::fromFile("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(Shader::fromFile("DeferredFinal.hlsl", ShaderType::Pixel)),
		visualVShader(Shader::fromFile("VoxelVisualVShader.hlsl", ShaderType::Vertex)),
		visualGShader(Shader::fromFile("VoxelVisualGShader.hlsl", ShaderType::Geometry)),
		visualPShader(Shader::fromFile("VoxelVisualPShader.hlsl", ShaderType::Pixel)),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f, 3.f)
	{
		{
			voxelParam = { 512,300.f };

			voxelTextureUint=new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, DXGI_FORMAT_R32_UINT);
			voxelTextureColor=new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, DXGI_FORMAT_R8G8B8A8_UNORM);

			voxelParamBuffer = new Buffer(sizeof(VoxelParam), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &voxelParam);
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

			Renderer::device->CreateInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(voxelVShader), inputLayout.ReleaseAndGetAddressOf());
		}

		{
			auto setLight = [](LightInfo::Light* light, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& color, const float& radius)
			{
				light->position = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 1.f);
				light->color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.f);
				light->radius = radius;
			};

			setLight(&lightInfo.lights[0], { -48.75f, 16.0f, -17.8f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[1], { -48.75f, 16.0f,  18.4f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[2], { 62.0f, 16.0f, -17.8f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[3], { 62.0f, 16.0f,  18.4f }, { 1.0f, 0.6f, 0.0f }, 45.0f);
			setLight(&lightInfo.lights[4], { 120.0f, 20.0f, -43.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[5], { 120.0f, 20.0f, 41.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[6], { -110.0f, 20.0f, -43.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[7], { -110.0f, 20.0f, 41.75f }, { 1.0f, 0.8f, 0.3f }, 75.0f);
			setLight(&lightInfo.lights[8], { -0.5f * 50.0f, 10.0f, 0.0f }, { 1.f,1.f,1.f }, 120.0f);
			setLight(&lightInfo.lights[9], { 0.75f * 50.0f, 10.0f, 0.0f }, { 1.f,1.f,1.f }, 120.0f);

			lightBuffer = new Buffer(sizeof(LightInfo), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &lightInfo);
		}

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 1000.f);

		camera.registerEvent();

		const DirectX::XMMATRIX voxelProj = DirectX::XMMatrixOrthographicOffCenterLH(
			-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f,
			-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f,
			-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f);

		memcpy(voxelProjBuffer->map(0).pData, &voxelProj, sizeof(DirectX::XMMATRIX));
		voxelProjBuffer->unmap(0);

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());

		const unsigned int color[4] = { 0,0,0,0 };

		voxelTextureUint->clear(color);
		RenderAPI::get()->OMSetUAV({ voxelTextureUint });

		RenderAPI::get()->VSSetBuffer({ voxelProjBuffer }, 2);
		RenderAPI::get()->PSSetBuffer({ lightBuffer,Camera::getViewBuffer(),voxelParamBuffer }, 1);
		RenderAPI::get()->PSSetSampler(States::get()->linearWrapSampler.GetAddressOf(), 0, 1);

		RenderAPI::get()->RSSetState(States::get()->rasterConserve.Get());
		RenderAPI::get()->OMSetDepthStencilState(States::get()->depthStencilDisable.Get(), 0);

		scene->drawVoxel(voxelVShader, voxelGShader, voxelPShader);

		voxelConvert->use();

		RenderAPI::get()->CSSetSRV({ voxelTextureUint }, 0);
		RenderAPI::get()->CSSetUAV({ voxelTextureColor }, 0);

		RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);
	}

	~MyGame()
	{
		delete shadowMap;
		delete voxelTextureUint;
		delete voxelTextureColor;
		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete scene;
		delete voxelParamBuffer;
		delete lightBuffer;
		delete voxelVShader;
		delete voxelGShader;
		delete voxelPShader;
		delete voxelConvert;
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;
		delete visualVShader;
		delete visualGShader;
		delete visualPShader;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());

		RenderAPI::get()->RSSetState(States::get()->rasterCullNone.Get());
		RenderAPI::get()->OMSetDepthStencilState(States::get()->defDepthStencilState.Get(), 0);

		RenderAPI::get()->UnbindVertexBuffer();

		shadowMap->clear();
		RenderAPI::get()->OMSetDefRTV(shadowMap->get());
		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Blue);

		RenderAPI::get()->GSSetSRV({ voxelTextureColor }, 0);
		RenderAPI::get()->GSSetBuffer({ voxelParamBuffer }, 2);
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

		visualVShader->use();
		visualGShader->use();
		visualPShader->use();

		RenderAPI::get()->Draw(voxelParam.voxelGridRes * voxelParam.voxelGridRes * voxelParam.voxelGridRes, 0);
	}


};