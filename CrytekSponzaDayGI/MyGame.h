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

	ComPtr<ID3D11SamplerState> shadowSampler;

	ShadowMap* shadowTexture;

	ShadowMap* shadowMap;

	RenderTexture* gPosition;

	RenderTexture* gNormalSpecular;

	RenderTexture* gBaseColor;

	RenderTexture* originTexture;

	ComputeTexture3D* voxelTextureTempColor;

	ComputeTexture3D* voxelTextureTempNormal;

	ComputeTexture3D* voxelTextureColor;

	ComputeTexture3D* voxelTextureNormal;

	ComputeTexture3D* voxelTextureColorFinal;

	Scene* scene;

	Buffer* voxelParamBuffer;

	Buffer* lightBuffer;

	Buffer* lightProjBuffer;

	Shader* voxelVShader;

	Shader* voxelGShader;

	Shader* voxelPShader;

	Shader* voxelConvert;

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

	static constexpr unsigned int shadowMapRes = 4096;

	struct VoxelParam
	{
		unsigned int voxelGridRes;
		float voxelGridLength;
		float voxelSize;
		float v0;
	} voxelParam{};

	struct Light
	{
		DirectX::XMFLOAT4 lightDir;
		DirectX::XMFLOAT4 lightColor;
	} light{};

	bool displayMode;

	float exposure;

	float gamma;

	MyGame() :
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
		shadowMap(new ShadowMap(Graphics::getWidth(), Graphics::getHeight())),
		shadowTexture(new ShadowMap(shadowMapRes, shadowMapRes)),
		scene(Scene::create(assetPath + "/sponza.dae")),
		voxelVShader(new Shader("VoxelizationVShader.hlsl", ShaderType::Vertex)),
		voxelGShader(new Shader("VoxelizationGShader.hlsl", ShaderType::Geometry)),
		voxelPShader(new Shader("VoxelizationPShader.hlsl", ShaderType::Pixel)),
		voxelConvert(new Shader("VoxelConvert.hlsl", ShaderType::Compute)),
		deferredVShader(new Shader("DeferredVShader.hlsl", ShaderType::Vertex)),
		deferredPShader(new Shader("DeferredPShader.hlsl", ShaderType::Pixel)),
		deferredFinal(new Shader("DeferredFinal.hlsl", ShaderType::Pixel)),
		visualVShader(new Shader("VoxelVisualVShader.hlsl", ShaderType::Vertex)),
		visualGShader(new Shader("VoxelVisualGShader.hlsl", ShaderType::Geometry)),
		visualPShader(new Shader("VoxelVisualPShader.hlsl", ShaderType::Pixel)),
		lightBounceCShader(new Shader("LightBounceCShader.hlsl", ShaderType::Compute)),
		skyboxPShader(new Shader("SkyboxPShader.hlsl", ShaderType::Pixel)),
		skybox(new TextureCube(assetPath + "/sky/kloppenheim_05_4k.hdr", 2048)),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f, 3.f),
		displayMode(false)
	{
		exposure = 0.60f;
		gamma = 1.25f;
		bloomEffect.setExposure(exposure);
		bloomEffect.setGamma(gamma);
		bloomEffect.applyChange();

		{
			voxelParam.voxelGridRes = 512;
			voxelParam.voxelGridLength = 300.f;
			voxelParam.voxelSize = voxelParam.voxelGridLength / (float)voxelParam.voxelGridRes;

			voxelTextureTempColor = new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, DXGI_FORMAT_R32_UINT);
			voxelTextureColor = new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET, D3D11_RESOURCE_MISC_GENERATE_MIPS, 5);

			voxelTextureTempNormal = new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, DXGI_FORMAT_R32_UINT);
			voxelTextureNormal = new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, DXGI_FORMAT_R16G16B16A16_SNORM);

			voxelTextureColorFinal = new ComputeTexture3D(voxelParam.voxelGridRes, voxelParam.voxelGridRes, voxelParam.voxelGridRes, DXGI_FORMAT_R8G8B8A8_UNORM, D3D11_BIND_RENDER_TARGET, D3D11_RESOURCE_MISC_GENERATE_MIPS, 5);

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

		camera.registerEvent();

		{
			const float radian = Math::half_pi - 0.25f;

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
			D3D11_SAMPLER_DESC desc = {};
			desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
			desc.BorderColor[0] = desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0.f;
			desc.ComparisonFunc = D3D11_COMPARISON_GREATER;
			desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			desc.MinLOD = 0.f;
			desc.MaxLOD = FLT_MAX;

			RenderAPI::get()->CreateSamplerState(desc, shadowSampler.ReleaseAndGetAddressOf());
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

		{
			const DirectX::XMMATRIX voxelProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f,
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f,
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f));

			Buffer* voxelProjBuffer = new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &voxelProj);

			const unsigned int color[4] = { 0,0,0,0 };
			voxelTextureTempColor->clear(color);
			voxelTextureTempNormal->clear(color);
			RenderAPI::get()->OMSetUAV({ voxelTextureTempColor,voxelTextureTempNormal });

			RenderAPI::get()->VSSetBuffer({ voxelProjBuffer }, 2);
			RenderAPI::get()->PSSetBuffer({ lightBuffer,Camera::getViewBuffer(),voxelParamBuffer,lightProjBuffer }, 1);
			RenderAPI::get()->PSSetSampler({ States::linearWrapSampler,shadowSampler.Get() }, 0);
			RenderAPI::get()->PSSetSRV({ shadowTexture }, 3);

			RenderAPI::get()->RSSetState(States::rasterConserve);
			RenderAPI::get()->OMSetDepthStencilState(States::depthStencilDisable, 0);

			scene->drawVoxel(voxelVShader, voxelGShader, voxelPShader);

			voxelConvert->use();

			RenderAPI::get()->CSSetSRV({ voxelTextureTempColor,voxelTextureTempNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColor,voxelTextureNormal }, 0);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColor->generateMips();

			lightBounceCShader->use();

			RenderAPI::get()->CSSetSRV({ voxelTextureColor,voxelTextureNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColorFinal }, 0);
			RenderAPI::get()->CSSetBuffer({ voxelParamBuffer }, 1);
			RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColorFinal->generateMips();

			RenderAPI::get()->CSSetSRV({ voxelTextureColorFinal,voxelTextureNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColor }, 0);
			RenderAPI::get()->CSSetBuffer({ voxelParamBuffer }, 1);
			RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColor->generateMips();

			RenderAPI::get()->CSSetSRV({ voxelTextureColor,voxelTextureNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColorFinal }, 0);
			RenderAPI::get()->CSSetBuffer({ voxelParamBuffer }, 1);
			RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColorFinal->generateMips();

			RenderAPI::get()->CSSetSRV({ voxelTextureColorFinal,voxelTextureNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColor }, 0);
			RenderAPI::get()->CSSetBuffer({ voxelParamBuffer }, 1);
			RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColor->generateMips();

			RenderAPI::get()->CSSetSRV({ voxelTextureColor,voxelTextureNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColorFinal }, 0);
			RenderAPI::get()->CSSetBuffer({ voxelParamBuffer }, 1);
			RenderAPI::get()->CSSetSampler({ States::linearClampSampler }, 0);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColorFinal->generateMips();

			delete voxelProjBuffer;

			RenderAPI::get()->OMSetDepthStencilState(States::defDepthStencilState, 0);
		}

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				displayMode = !displayMode;
			});

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

		delete voxelTextureTempColor;
		delete voxelTextureColor;
		delete voxelTextureTempNormal;
		delete voxelTextureNormal;
		delete voxelTextureColorFinal;
		delete voxelParamBuffer;

		delete voxelConvert;
		delete lightBounceCShader;

		delete voxelVShader;
		delete voxelGShader;
		delete voxelPShader;

		delete visualVShader;
		delete visualGShader;
		delete visualPShader;

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
		if (displayMode)
		{
			RenderAPI::get()->RSSetState(States::rasterCullBack);

			RenderAPI::get()->UnbindVertexBuffer();

			shadowMap->clear();
			RenderAPI::get()->OMSetDefRTV(shadowMap);
			RenderAPI::get()->ClearDefRTV(DirectX::Colors::Blue);

			RenderAPI::get()->GSSetSRV({ voxelTextureColorFinal }, 0);
			RenderAPI::get()->GSSetBuffer({ voxelParamBuffer }, 2);
			RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

			visualVShader->use();
			visualGShader->use();
			visualPShader->use();

			RenderAPI::get()->Draw(voxelParam.voxelGridRes * voxelParam.voxelGridRes * voxelParam.voxelGridRes, 0);
		}
		else
		{
			RenderAPI::get()->RSSetState(States::rasterCullBack);

			RenderAPI::get()->GSSetShader(nullptr);
			RenderAPI::get()->PSSetSampler({ States::linearWrapSampler,States::linearClampSampler,shadowSampler.Get() }, 0);

			gBaseColor->clearRTV(DirectX::Colors::Black);
			gPosition->clearRTV(DirectX::Colors::Black);
			gNormalSpecular->clearRTV(DirectX::Colors::Black);
			shadowMap->clear(D3D11_CLEAR_DEPTH);

			RenderAPI::get()->OMSetRTV({ gPosition,gNormalSpecular,gBaseColor }, shadowMap);

			scene->draw(deferredVShader, deferredPShader);

			originTexture->clearRTV(DirectX::Colors::Black);
			RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
			RenderAPI::get()->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoEffect.process(shadowMap->getSRV(), gNormalSpecular->getSRV()),shadowTexture,voxelTextureColorFinal }, 0);
			RenderAPI::get()->PSSetBuffer({ Camera::getViewBuffer(),lightBuffer,voxelParamBuffer,lightProjBuffer }, 1);

			RenderAPI::fullScreenVS->use();
			deferredFinal->use();

			RenderAPI::get()->DrawQuad();

			RenderAPI::get()->OMSetRTV({ originTexture }, shadowMap);
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