#pragma once

#include<Aurora/EngineAPI/RenderPass.h>

#include<Aurora/Resource/RenderTexture.h>
#include<Aurora/Resource/DepthTexture.h>
#include<Aurora/Resource/TextureCube.h>
#include<Aurora/Resource/ComputeTexture.h>

#include<Aurora/EngineAPI/Camera.h>

class OceanRenderPass :public RenderPass
{
public:

	ComPtr<ID3D11InputLayout> inputLayout;

	Shader* skyboxPS;

	RenderTexture* originTexture;

	DepthTexture* depthTexture;

	TextureCube* textureCube;

	ResourceTexture* perlinTexture;

	ResourceTexture* gaussTexture;

	ComputeTexture* tildeh0k;

	ComputeTexture* tildeh0;

	ComputeTexture* waveData;

	ComputeTexture* tempTexture;

	ComputeTexture* Dy;

	ComputeTexture* Dx;

	ComputeTexture* Dz;

	ComputeTexture* Dyx;

	ComputeTexture* Dyz;

	ComputeTexture* Dxx;

	ComputeTexture* Dzz;

	ComputeTexture* Dxz;

	ComputeTexture* Dxyz;

	ComputeTexture* normalJacobian;

	VertexBuffer* patchVertexBuffer;

	ConstantBuffer* oceanParamBuffer;

	Shader* phillipSpectrumShader;

	Shader* conjugatedCalcCS;

	Shader* displacementShader;

	Shader* ifftShader;

	Shader* permutationCS;

	Shader* waveMergeCS;

	Shader* oceanVShader;

	Shader* oceanHShader;

	Shader* oceanDShader;

	Shader* oceanPShader;

	static constexpr unsigned int patchSize = 32;

	struct Param
	{
		unsigned int mapResolution = 1024;
		float mapLength = 512.0;
		DirectX::XMFLOAT2 wind = { 15.f,0.f };
		float amplitude = 0.000003f;
		float gravity = 9.81f;
		DirectX::XMFLOAT2 v0 = { 0.f,0.f };
	}param;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	//风或者引力常量变化都要重新计算phillipTexture
	void calculatePhillipTexture() const
	{
		memcpy(ImCtx::get()->Map(oceanParamBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &param, sizeof(Param));
		ImCtx::get()->Unmap(oceanParamBuffer, 0);

		ImCtx::get()->CSSetConstantBuffer({ oceanParamBuffer }, 1);
		ImCtx::get()->CSSetSRV({ gaussTexture }, 0);
		ImCtx::get()->CSSetUAV({ tildeh0k->getMip(0),waveData->getMip(0) }, 0);

		ImCtx::get()->BindShader(phillipSpectrumShader);

		ImCtx::get()->Dispatch(1024 / 32u, 1024 / 32u, 1u);

		ImCtx::get()->CSSetSRV({ tildeh0k }, 0);
		ImCtx::get()->CSSetUAV({ tildeh0->getMip(0) }, 0);

		ImCtx::get()->BindShader(conjugatedCalcCS);

		ImCtx::get()->Dispatch(1024 / 32u, 1024 / 32u, 1u);
	}

	OceanRenderPass(RenderTexture* originTexture) :
		originTexture(new RenderTexture(*originTexture)),
		depthTexture(new DepthTexture(Graphics::getWidth(), Graphics::getHeight(), FMT::D32F, false)),
		skyboxPS(new Shader("SkyboxPS.hlsl", ShaderType::Pixel)),
		textureCube(new TextureCube("ColdSunsetEquirect.png", 2048)),
		perlinTexture(new ResourceTexture("PerlinNoise.dds")),
		oceanParamBuffer(new ConstantBuffer(sizeof(Param), D3D11_USAGE_DYNAMIC)),
		gaussTexture(new ResourceTexture(1024, 1024, Texture2D::TextureType::Gauss)),
		tildeh0k(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		tildeh0(new ComputeTexture(1024, 1024, FMT::RGBA32F, FMT::RGBA32F, FMT::RGBA32F)),
		waveData(new ComputeTexture(1024, 1024, FMT::RGBA32F, FMT::RGBA32F, FMT::RGBA32F)),
		Dy(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		Dx(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		Dz(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		Dyx(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		Dyz(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		Dxx(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		Dzz(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		Dxz(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		tempTexture(new ComputeTexture(1024, 1024, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
		Dxyz(new ComputeTexture(1024, 1024, FMT::RGBA32F, FMT::RGBA32F, FMT::RGBA32F)),
		normalJacobian(new ComputeTexture(1024, 1024, FMT::RGBA32F, FMT::RGBA32F, FMT::RGBA32F)),
		phillipSpectrumShader(new Shader("PhillipsSpectrum.hlsl", ShaderType::Compute)),
		conjugatedCalcCS(new Shader("ConjugatedCalcCS.hlsl", ShaderType::Compute)),
		displacementShader(new Shader("Displacement.hlsl", ShaderType::Compute)),
		ifftShader(new Shader("IFFT.hlsl", ShaderType::Compute)),
		permutationCS(new Shader("PermutationCS.hlsl", ShaderType::Compute)),
		waveMergeCS(new Shader("WaveMergeCS.hlsl", ShaderType::Compute)),
		oceanVShader(new Shader("OceanVShader.hlsl", ShaderType::Vertex)),
		oceanHShader(new Shader("OceanHShader.hlsl", ShaderType::Hull)),
		oceanDShader(new Shader("OceanDShader.hlsl", ShaderType::Domain)),
		oceanPShader(new Shader("OceanPShader.hlsl", ShaderType::Pixel))
	{
		float clearValue[] = { 999,999,999,999 };
		ImCtx::get()->ClearUAV(normalJacobian->getMip(0), clearValue);

		{
			D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[2] =
			{
				{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
				{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
			};

			GraphicsDevice::get()->createInputLayout(inputLayoutDesc, ARRAYSIZE(inputLayoutDesc), SHADERDATA(oceanVShader), inputLayout.ReleaseAndGetAddressOf());
		}

		{
			std::vector<Vertex> vertices;

			auto getVertexAt = [this](const int& x, const int& z)
			{
				const float xPos = ((float)x - (float)patchSize / 2.f) * (float)param.mapLength / (float)patchSize - (float)param.mapLength / 2.f;
				const float zPos = ((float)z - (float)patchSize / 2.f) * (float)param.mapLength / (float)patchSize - (float)param.mapLength / 2.f;
				const float u = (float)x / (float)patchSize;
				const float v = (float)z / (float)patchSize;
				return Vertex{ {xPos,0.f,zPos},{u,v} };
			};

			for (int z = 0; z < 256; z++)
			{
				for (int x = 0; x < 256; x++)
				{
					vertices.push_back(getVertexAt(x, z));
					vertices.push_back(getVertexAt(x, z + 1));
					vertices.push_back(getVertexAt(x + 1, z + 1));
					vertices.push_back(getVertexAt(x + 1, z));
				}
			}

			patchVertexBuffer = new VertexBuffer(sizeof(Vertex) * vertices.size(), D3D11_USAGE_IMMUTABLE, vertices.data());
		}

		calculatePhillipTexture();

		ImCtx::get()->ResetStates();
	}

	~OceanRenderPass()
	{
		delete skyboxPS;
		delete originTexture;
		delete depthTexture;
		delete textureCube;
		delete perlinTexture;

		delete oceanParamBuffer;
		delete patchVertexBuffer;

		delete phillipSpectrumShader;
		delete conjugatedCalcCS;
		delete displacementShader;
		delete ifftShader;
		delete permutationCS;
		delete waveMergeCS;
		delete oceanVShader;
		delete oceanHShader;
		delete oceanDShader;
		delete oceanPShader;

		delete gaussTexture;
		delete tildeh0k;
		delete tildeh0;
		delete waveData;
		delete Dy;
		delete Dx;
		delete Dz;
		delete Dyx;
		delete Dyz;
		delete Dxx;
		delete Dzz;
		delete Dxz;
		delete Dxyz;
		delete normalJacobian;
		delete tempTexture;
	}

	void recordCommand() override
	{
		context->PSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);
		context->CSSetConstantBuffer({ Graphics::getDeltaTimeBuffer() }, 0);

		context->VSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->HSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->DSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);
		context->GSSetConstantBuffer({ Camera::getProjBuffer(),Camera::getViewBuffer() }, 0);

		context->CSSetConstantBuffer({ oceanParamBuffer }, 1);

		context->CSSetSRV({ tildeh0,waveData }, 0);
		context->CSSetUAV({ Dy->getMip(0),Dx->getMip(0),Dz->getMip(0),Dyx->getMip(0),Dyz->getMip(0),Dxx->getMip(0),Dzz->getMip(0),Dxz->getMip(0) }, 0);

		context->BindShader(displacementShader);

		context->Dispatch(1024 / 32u, 1024 / 32u, 1u);

		IFFT(Dy);
		IFFT(Dx);
		IFFT(Dz);
		IFFT(Dyx);
		IFFT(Dyz);
		IFFT(Dxx);
		IFFT(Dzz);
		IFFT(Dxz);

		context->CSSetSRV({ Dy,Dx,Dz,Dyx,Dyz,Dxx,Dzz,Dxz }, 0);
		context->CSSetUAV({ Dxyz->getMip(0),normalJacobian->getMip(0) }, 0);

		context->BindShader(waveMergeCS);
		context->Dispatch(1024 / 32u, 1024 / 32u, 1u);

		context->RSSetState(States::rasterCullBack);
		context->OMSetDepthStencilState(States::defDepthStencilState, 0);

		context->ClearRTV(originTexture->getMip(0), DirectX::Colors::Black);
		context->OMSetRTV({ originTexture->getMip(0) }, nullptr);
		context->RSSetViewport(Graphics::getWidth(), Graphics::getHeight());

		context->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		context->PSSetSRV({ textureCube }, 0);
		context->PSSetSampler({ States::linearWrapSampler }, 0);

		context->BindShader(Shader::skyboxVS);
		context->BindShader(skyboxPS);
		context->HSUnbindShader();
		context->GSUnbindShader();

		context->DrawCube();

		context->ClearDSV(depthTexture, D3D11_CLEAR_DEPTH);
		context->OMSetRTV({ originTexture->getMip(0) }, depthTexture);

		context->PSSetSRV({ textureCube,perlinTexture }, 1);

		context->IASetInputLayout(inputLayout.Get());
		context->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		context->IASetVertexBuffer({ patchVertexBuffer }, { sizeof(Vertex) }, { 0 });

		context->BindShader(oceanVShader);
		context->BindShader(oceanHShader);
		context->BindShader(oceanDShader);
		context->BindShader(oceanPShader);

		context->DSSetSRV({ Dxyz }, 0);
		context->PSSetSRV({ normalJacobian }, 0);

		context->PSSetSampler({ States::linearWrapSampler }, 0);
		context->DSSetSampler({ States::linearWrapSampler }, 0);

		context->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);

		context->Draw(4 * 256 * 256, 0);

		context->HSUnbindShader();
		context->DSUnbindShader();
	}

	void IFFT(ComputeTexture* const cTexture) const
	{
		context->CSSetUAV({ tempTexture->getMip(0) }, 0);
		context->CSSetSRV({ cTexture }, 0);

		context->BindShader(ifftShader);
		context->Dispatch(1024, 1u, 1u);

		context->CSSetUAV({ cTexture->getMip(0) }, 0);
		context->CSSetSRV({ tempTexture }, 0);

		context->BindShader(ifftShader);
		context->Dispatch(1024, 1u, 1u);

		context->BindShader(permutationCS);
		context->Dispatch(1024 / 32u, 1024 / 32u, 1u);
	}

};