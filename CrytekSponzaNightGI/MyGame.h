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
		gPosition(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gNormalSpecular(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::Colors::Black)),
		gBaseColor(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::Colors::Black)),
		originTexture(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::Colors::Black)),
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
		lightBounceCShader(Shader::fromFile("LightBounceCShader.hlsl", ShaderType::Compute)),
		skyboxPShader(Shader::fromFile("SkyboxPShader.hlsl", ShaderType::Pixel)),
		skybox(TextureCube::create({ assetPath + "/sky/SkyEarlyDusk_Right.png",assetPath + "/sky/SkyEarlyDusk_Left.png",assetPath + "/sky/SkyEarlyDusk_Top.png",assetPath + "/sky/SkyEarlyDusk_Bottom.png",assetPath + "/sky/SkyEarlyDusk_Front.png",assetPath + "/sky/SkyEarlyDusk_Back.png" })),
		hbaoEffect(Graphics::getWidth(), Graphics::getHeight()),
		bloomEffect(Graphics::getWidth(), Graphics::getHeight()),
		camera({ 0.0f,20.f,0.f }, { 1.0f,0.f,0.f }, { 0.f,1.f,0.f }, 100.f, 3.f),
		displayMode(false)
	{
		exposure = 0.40f;
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

			lightBuffer = new Buffer(sizeof(LightInfo), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &lightInfo);
		}

		Camera::setProj(Math::pi / 4.f, Graphics::getAspectRatio(), 1.f, 512.f);

		camera.registerEvent();

		{
			const DirectX::XMMATRIX voxelProj = DirectX::XMMatrixTranspose(DirectX::XMMatrixOrthographicOffCenterLH(
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f,
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f,
				-(float)voxelParam.voxelGridLength / 2.f, (float)voxelParam.voxelGridLength / 2.f));

			Buffer* voxelProjBuffer = new Buffer(sizeof(DirectX::XMMATRIX), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_IMMUTABLE, &voxelProj);

			RenderAPI::get()->OMSetBlendState(nullptr);
			RenderAPI::get()->IASetInputLayout(inputLayout.Get());

			const unsigned int color[4] = { 0,0,0,0 };
			voxelTextureTempColor->clear(color);
			voxelTextureTempNormal->clear(color);
			RenderAPI::get()->OMSetUAV({ voxelTextureTempColor,voxelTextureTempNormal });

			RenderAPI::get()->VSSetBuffer({ voxelProjBuffer }, 2);
			RenderAPI::get()->PSSetBuffer({ lightBuffer,Camera::getViewBuffer(),voxelParamBuffer }, 1);
			RenderAPI::get()->PSSetSampler(States::get()->linearWrapSampler.GetAddressOf(), 0, 1);

			RenderAPI::get()->RSSetState(States::get()->rasterConserve.Get());
			RenderAPI::get()->OMSetDepthStencilState(States::get()->depthStencilDisable.Get(), 0);

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
			RenderAPI::get()->CSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColorFinal->generateMips();

			RenderAPI::get()->CSSetSRV({ voxelTextureColorFinal,voxelTextureNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColor }, 0);
			RenderAPI::get()->CSSetBuffer({ voxelParamBuffer }, 1);
			RenderAPI::get()->CSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColor->generateMips();

			RenderAPI::get()->CSSetSRV({ voxelTextureColor,voxelTextureNormal }, 0);
			RenderAPI::get()->CSSetUAV({ voxelTextureColorFinal }, 0);
			RenderAPI::get()->CSSetBuffer({ voxelParamBuffer }, 1);
			RenderAPI::get()->CSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);

			RenderAPI::get()->Dispatch(voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8, voxelParam.voxelGridRes / 8);

			voxelTextureColorFinal->generateMips();

			delete voxelProjBuffer;
		}

		RenderAPI::get()->OMSetBlendState(nullptr);
		RenderAPI::get()->IASetInputLayout(inputLayout.Get());
		RenderAPI::get()->RSSetState(States::get()->rasterCullBack.Get());
		RenderAPI::get()->OMSetDepthStencilState(States::get()->defDepthStencilState.Get(), 0);

		Keyboard::addKeyDownEvent(Keyboard::K, [this]()
			{
				displayMode = !displayMode;
			});

		Keyboard::addKeyDownEvent(Keyboard::J, [this]()
			{
				const DirectX::XMFLOAT3 viewPos = camera.getEye();
				std::cout << viewPos.x << " " << viewPos.y << " " << viewPos.z << "\n";
			});
	}

	~MyGame()
	{
		delete shadowMap;

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
			RenderAPI::get()->RSSetState(States::get()->rasterCullBack.Get());

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
			RenderAPI::get()->RSSetState(States::get()->rasterCullBack.Get());

			RenderAPI::get()->GSSetShader(nullptr);
			ID3D11SamplerState* samplers[2] = { States::get()->linearWrapSampler.Get(),States::get()->linearClampSampler.Get() };
			RenderAPI::get()->PSSetSampler(samplers, 0, 2);

			gBaseColor->clearRTV(DirectX::Colors::Black);
			gPosition->clearRTV(DirectX::Colors::Black);
			gNormalSpecular->clearRTV(DirectX::Colors::Black);
			shadowMap->clear(D3D11_CLEAR_DEPTH);

			RenderAPI::get()->OMSetRTV({ gPosition,gNormalSpecular,gBaseColor }, shadowMap);

			scene->draw(deferredVShader, deferredPShader);

			originTexture->clearRTV(DirectX::Colors::Black);
			RenderAPI::get()->OMSetRTV({ originTexture }, nullptr);
			RenderAPI::get()->PSSetSRV({ gPosition,gNormalSpecular,gBaseColor,hbaoEffect.process(shadowMap->getSRV(), gNormalSpecular->getSRV()),voxelTextureColorFinal }, 0);
			RenderAPI::get()->PSSetBuffer({ Camera::getViewBuffer(),lightBuffer,voxelParamBuffer }, 1);

			Shader::displayVShader->use();
			deferredFinal->use();

			RenderAPI::get()->DrawQuad();

			RenderAPI::get()->OMSetRTV({ originTexture }, shadowMap);
			RenderAPI::get()->PSSetSRV({ skybox }, 0);

			TextureCube::shader->use();
			skyboxPShader->use();

			RenderAPI::get()->DrawCube();

			ShaderResourceView* const bloomTextureSRV = bloomEffect.process(originTexture);

			RenderAPI::get()->OMSetBlendState(nullptr);
			RenderAPI::get()->OMSetDefRTV(nullptr);
			RenderAPI::get()->PSSetSRV({ bloomTextureSRV }, 0);

			Shader::displayVShader->use();
			Shader::displayPShader->use();

			RenderAPI::get()->DrawQuad();
		}

	}


};