#pragma once

#include<Aurora/Game.h>

#include<Aurora/Camera/FPSCamera.h>
#include<Aurora/EngineAPI/ResourceEssential.h>

#include<Aurora/Effect/HBAOEffect.h>
#include<Aurora/Effect/BloomEffect.h>

#include"Scene.h"

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	ResourceDepthTexture* resDepthTexture;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	RenderTexture* originTexture;

	ComputeTexture3D* voxelTextureColor;

	ComputeTexture3D* voxelTextureColorFinal;

	ComputeTexture3D* voxelTextureNormal;

	Scene* scene;

	ConstantBuffer* voxelParamBuffer;

	ConstantBuffer* lightBuffer;

	Shader* voxelVShader;

	Shader* voxelGShader;

	Shader* voxelPShader;

	Shader* lightBounceCShader;

	Shader* deferredVShader;

	Shader* deferredPShader;

	Shader* deferredFinal;

	Shader* visualVShader;

	Shader* visualGShader;

	Shader* visualPShader;

	Shader* skyboxPShader;

	FPSCamera camera;

	HBAOEffect hbaoEffect;

	BloomEffect bloomEffect;

	TextureCube* skybox;

	struct VoxelParam
	{
		unsigned int voxelGridRes;
		float voxelGridLength;
		float voxelSize;
		float v0;
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

	bool displayMode;

	float exposure;

	float gamma;

	MyGame() :
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA32F, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA32F, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA8UN, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::RGBA16F, DirectX::Colors::Black)),
		resDepthTexture(new ResourceDepthTexture(Graphics::getWidth(), Graphics::getHeight())),
		scene(Scene::create(assetPath + "/sponza.dae")),
		voxelVShader(new Shader("VoxelizationVShader.hlsl", ShaderType::Vertex)),
		voxelGShader(new Shader("VoxelizationGShader.hlsl", ShaderType::Geometry)),
		voxelPShader(new Shader("VoxelizationPShader.hlsl", ShaderType::Pixel)),
		deferredVShader(new Shader("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(new Shader("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(new Shader("DeferredFinal.hlsl", ShaderType::Pixel)),
		visualVShader(new Shader("VoxelVisualVShader.hlsl", ShaderType::Vertex)),
		visualGShader(new Shader("VoxelVisualGShader.hlsl", ShaderType::Geometry)),
		visualPShader(new Shader("VoxelVisualPShader.hlsl", ShaderType::Pixel)),
		lightBounceCShader(new Shader("LightBounceCShader.hlsl", ShaderType::Compute)),
		skyboxPShader(new Shader("SkyboxPShader.hlsl", ShaderType::Pixel)),
		skybox(new TextureCube({ assetPath + "/sky/SkyEarlyDusk_Right.png",assetPath + "/sky/SkyEarlyDusk_Left.png",assetPath + "/sky/SkyEarlyDusk_Top.png",assetPath + "/sky/SkyEarlyDusk_Bottom.png",assetPath + "/sky/SkyEarlyDusk_Front.png",assetPath + "/sky/SkyEarlyDusk_Back.png" })),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f),
		displayMode(false)
	{
		exposure = 1.f;
		gamma = 1.f;
		bloomEffect.setExposure(exposure);
		bloomEffect.setGamma(gamma);
		bloomEffect.setThreshold(0.5f);
		bloomEffect.applyChange();

		{
			voxelParam.voxelGridRes = 512;
			voxelParam.voxelGridLength = 350.f;
			voxelParam.voxelSize = voxelParam.voxelGridLength / (float)voxelParam.voxelGridRes;

			voxelTextureColor = new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, FMT::RGBA8TL, FMT::RGBA8UN, FMT::R32UI, D3D11_BIND_RENDER_TARGET, D3D11_RESOURCE_MISC_GENERATE_MIPS, 5);
			voxelTextureColorFinal = new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, FMT::RGBA8UN, D3D11_BIND_RENDER_TARGET, D3D11_RESOURCE_MISC_GENERATE_MIPS, 5);
			voxelTextureNormal = new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, FMT::RGBA8TL, FMT::RGBA8UN, FMT::R32UI);

			voxelParamBuffer = new ConstantBuffer(sizeof(VoxelParam), D3D11_USAGE_IMMUTABLE, &voxelParam);
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

			Renderer::get()->createInputLayout(layout, ARRAYSIZE(layout), SHADERDATA(voxelVShader), inputLayout.ReleaseAndGetAddressOf());
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
			setLight(&lightInfo.lights[8], { -0.5f * 50.0f, 20.0f, 0.0f }, { 1.f,1.f,1.f }, 120.0f);
			setLight(&lightInfo.lights[9], { 0.75f * 50.0f, 20.0f, 0.0f }, { 1.f,1.f,1.f }, 120.0f);

			lightBuffer = new ConstantBuffer(sizeof(LightInfo), D3D11_USAGE_IMMUTABLE, &lightInfo);
		}

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);

		camera.registerEvent();

		{
			const DirectX::XMMATRIX voxelProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f,
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f,
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f));

			ConstantBuffer* voxelProjBuffer = new ConstantBuffer(sizeof(DirectX::XMMATRIX), D3D11_USAGE_IMMUTABLE, &voxelProj);

			RenderAPI::get()->OMSetBlendState(nullptr);
			RenderAPI::get()->IASetInputLayout(inputLayout.Get());

			const unsigned int color[4] = { 0,0,0,0 };
			voxelTextureColor->clearUAV(color);
			voxelTextureNormal->clearUAV(color);
			RenderAPI::get()->OMSetUAV({ voxelTextureColor,voxelTextureNormal });

			RenderAPI::get()->VSSetConstantBuffer({ voxelProjBuffer }, 2);
			RenderAPI::get()->PSSetConstantBuffer({ lightBuffer,Camera::getViewBuffer(),voxelParamBuffer }, 1);
			RenderAPI::get()->PSSetSampler({ States::linearWrapSampler }, 0);

			RenderAPI::get()->RSSetState(States::rasterConserve);
			RenderAPI::get()->OMSetDepthStencilState(States::depthStencilDisable, 0);

			scene->drawVoxel(voxelVShader, voxelGShader, voxelPShader);

			voxelTextureColor->generateMips();

			delete voxelProjBuffer;

			lightBounceCShader->use();

			RenderAPI::get()->CSSetSRV({ voxelTextureColor,voxelTextureNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColorFinal }, 0);
			RenderAPI::get()->CSSetConstantBuffer({ voxelParamBuffer }, 1);
			RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColorFinal->generateMips();

		}

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());
		RenderAPI::get()->RSSetState(States::rasterCullBack);
		RenderAPI::get()->OMSetDepthStencilState(States::defDepthStencilState, 0);

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				displayMode = !displayMode;
			});

	}

	~MyGame()
	{
		delete resDepthTexture;

		delete gPosition;
		delete gNormalSpecular;
		delete gBaseColor;
		delete originTexture;

		delete scene;
		delete skybox;

		delete lightBuffer;
		delete deferredVShader;
		delete deferredPShader;
		delete deferredFinal;

		delete voxelTextureColor;
		delete voxelTextureNormal;
		delete voxelTextureColorFinal;
		delete voxelParamBuffer;

		delete lightBounceCShader;

		delete voxelVShader;
		delete voxelGShader;
		delete voxelPShader;

		delete visualVShader;
		delete visualGShader;
		delete visualPShader;

		delete skyboxPShader;
	}

	void imGUICall() override
	{
		bloomEffect.imGUIEffectModifier();
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
		if (displayMode)
		{
			RenderAPI::get()->RSSetState(States::rasterCullNone);

			resDepthTexture->clearDSV(D3D11_CLEAR_DEPTH);
			RenderAPI::get()->OMSetDefRTV(resDepthTexture);
			RenderAPI::get()->ClearDefRTV(DirectX::Colors::Blue);

			RenderAPI::get()->GSSetSRV({ voxelTextureColorFinal }, 0);
			RenderAPI::get()->GSSetConstantBuffer({ voxelParamBuffer }, 2);
			RenderAPI::get()->GSSetSampler({ States::pointClampSampler }, 0);
			RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			visualVShader->use();
			visualGShader->use();
			visualPShader->use();

			RenderAPI::get()->Draw(voxelParam.voxelGridRes * voxelParam.voxelGridRes * voxelParam.voxelGridRes, 0);
		}
		else
		{
			RenderAPI::get()->RSSetState(States::rasterCullBack);

			RenderAPI::get()->GSUnbindShader();
			RenderAPI::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler }, 0);

			gBaseColor->clearRTV(DirectX::Colors::Black, 0);
			gPosition->clearRTV(DirectX::Colors::Black, 0);
			gNormalSpecular->clearRTV(DirectX::Colors::Black, 0);
			resDepthTexture->clearDSV(D3D11_CLEAR_DEPTH);

			RenderAPI::get()->OMSetRTV({ gPosition->getRTVMip(0),gNormalSpecular->getRTVMip(0),gBaseColor->getRTVMip(0) }, resDepthTexture);

			scene->draw(deferredVShader, deferredPShader);

			originTexture->clearRTV(DirectX::Colors::Black, 0);
			RenderAPI::get()->OMSetRTV({ originTexture->getRTVMip(0) }, nullptr);
			RenderAPI::get()->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoEffect.process(resDepthTexture->getSRV(), gNormalSpecular->getSRV()),voxelTextureColorFinal }, 0);
			RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer(),lightBuffer,voxelParamBuffer }, 1);

			RenderAPI::fullScreenVS->use();
			deferredFinal->use();

			RenderAPI::get()->DrawQuad();

			RenderAPI::get()->OMSetRTV({ originTexture->getRTVMip(0) }, resDepthTexture);
			RenderAPI::get()->PSSetSRV({ skybox }, 0);

			RenderAPI::skyboxVS->use();
			skyboxPShader->use();

			RenderAPI::get()->DrawCube();

			ShaderResourceView* const bloomTextureSRV = bloomEffect.process(originTexture);

			RenderAPI::get()->OMSetBlendState(nullptr);
			RenderAPI::get()->OMSetDefRTV(nullptr);
			RenderAPI::get()->PSSetSRV({ bloomTextureSRV }, 0);

			RenderAPI::fullScreenVS->use();
			RenderAPI::fullScreenPS->use();

			RenderAPI::get()->DrawQuad();
		}

	}


};