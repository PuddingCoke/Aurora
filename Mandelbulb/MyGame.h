#pragma once

#include<Aurora/Game.h>
#include<Aurora/ComputeTexture3D.h>
#include<Aurora/RenderAPI.h>
#include<Aurora/A3D/OrbitCamera.h>

//这是一个模板项目，在项目选项中选择导出模板即可
class MyGame :public Game
{
public:

	ComputeTexture3D* mandelTexture;

	Shader* mandelCompute;

	Shader* modelVShader;

	Shader* modelPShader;

	Shader* rayCastVShader;

	Shader* rayCastPSHader;

	RenderTexture* modelCullFront;

	RenderTexture* modelCullBack;

	OrbitCamera camera;

	ComPtr<ID3D11SamplerState> borderSampler;

	MyGame() :
		mandelTexture(new ComputeTexture3D(512, 512, 512, DXGI_FORMAT_R8G8B8A8_UNORM)),
		mandelCompute(new Shader("MandelbulbCompute.hlsl", ShaderType::Compute)),
		modelCullFront(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		modelCullBack(new RenderTexture(Graphics::getWidth(), Graphics::getHeight(), DXGI_FORMAT_R16G16B16A16_FLOAT)),
		modelVShader(new Shader("ModelVShader.hlsl", ShaderType::Vertex)),
		modelPShader(new Shader("ModelPShader.hlsl", ShaderType::Pixel)),
		rayCastVShader(new Shader("RayCastVShader.hlsl", ShaderType::Vertex)),
		rayCastPSHader(new Shader("RayCastPShader.hlsl", ShaderType::Pixel)),
		camera({ 12,0,-8 }, { 0,0,-1 })
	{
		camera.registerEvent();

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0.f;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		RenderAPI::get()->CreateSamplerState(samplerDesc, borderSampler.GetAddressOf());

		RenderAPI::get()->CSSetUAV({ mandelTexture }, 0);
		mandelCompute->use();
		RenderAPI::get()->Dispatch(mandelTexture->width / 8, mandelTexture->height / 8, mandelTexture->depth / 8);
	}

	~MyGame()
	{
		delete mandelCompute;
		delete modelVShader;
		delete modelPShader;
		delete rayCastVShader;
		delete rayCastPSHader;
		delete mandelTexture;
		delete modelCullFront;
		delete modelCullBack;
	}

	void update(const float& dt) override
	{
		camera.applyInput(dt);
	}

	void render()
	{
		RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		modelVShader->use();
		modelPShader->use();

		modelCullFront->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ modelCullFront }, nullptr);
		RenderAPI::get()->RSSetState(States::get()->rasterCullFront.Get());

		RenderAPI::get()->DrawCube();

		modelCullBack->clearRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetRTV({ modelCullBack }, nullptr);
		RenderAPI::get()->RSSetState(States::get()->rasterCullBack.Get());

		RenderAPI::get()->DrawCube();

		RenderAPI::get()->ClearDefRTV(DirectX::Colors::Black);
		RenderAPI::get()->OMSetDefRTV(nullptr);
		RenderAPI::get()->PSSetSRV({ mandelTexture,modelCullBack,modelCullFront }, 0);
		RenderAPI::get()->PSSetSampler({ borderSampler.Get() }, 0);

		rayCastVShader->use();
		rayCastPSHader->use();

		RenderAPI::get()->DrawCube();
	}


};