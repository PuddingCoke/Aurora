#pragma once

#include<Aurora/EngineAPI/ImCtx.h>
#include<Aurora/EngineAPI/States.h>
#include<Aurora/Core/Shader.h>
#include<Aurora/EngineAPI/ResourceEssential.h>

class Ocean
{
public:

	Ocean(const unsigned int& mapResolution, const float& mapLength, const DirectX::XMFLOAT2& wind, const float& phillipParam);

	~Ocean();

	void update() const;

	void render() const;

private:

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

	ComPtr<ID3D11InputLayout> inputLayout;

	ComPtr<ID3D11RasterizerState> wireState;

	static constexpr unsigned int patchSize = 32;

	struct Param
	{
		unsigned int mapResolution;
		float mapLength;
		DirectX::XMFLOAT2 wind;
		float amplitude;
		float gravity;
		DirectX::XMFLOAT2 v0;
	}param;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	//风或者引力常量变化都要重新计算phillipTexture
	void calculatePhillipTexture() const;

	void IFFT(ComputeTexture* const cTexture) const;
};

inline Ocean::Ocean(const unsigned int& mapResolution, const float& mapLength, const DirectX::XMFLOAT2& wind, const float& phillipParam) :
	oceanParamBuffer(new ConstantBuffer(sizeof(Param), D3D11_USAGE_DYNAMIC)),
	param{ mapResolution, mapLength, wind, phillipParam, 9.81f },
	gaussTexture(new ResourceTexture(mapResolution, mapResolution, Texture2D::TextureType::Gauss)),
	tildeh0k(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	tildeh0(new ComputeTexture(mapResolution, mapResolution, FMT::RGBA32F, FMT::RGBA32F, FMT::RGBA32F)),
	waveData(new ComputeTexture(mapResolution, mapResolution, FMT::RGBA32F, FMT::RGBA32F, FMT::RGBA32F)),
	Dy(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	Dx(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	Dz(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	Dyx(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	Dyz(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	Dxx(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	Dzz(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	Dxz(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	tempTexture(new ComputeTexture(mapResolution, mapResolution, FMT::RG32F, FMT::RG32F, FMT::RG32F)),
	Dxyz(new ComputeTexture(mapResolution, mapResolution, FMT::RGBA32F, FMT::RGBA32F, FMT::RGBA32F)),
	normalJacobian(new ComputeTexture(mapResolution, mapResolution, FMT::RGBA32F, FMT::RGBA32F, FMT::RGBA32F)),
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

	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.CullMode = D3D11_CULL_BACK;
		desc.FillMode = D3D11_FILL_WIREFRAME;

		GraphicsDevice::get()->createRasterizerState(&desc, wireState.ReleaseAndGetAddressOf());
	}

	calculatePhillipTexture();
}

inline Ocean::~Ocean()
{
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

inline void Ocean::calculatePhillipTexture() const
{
	memcpy(ImCtx::get()->Map(oceanParamBuffer, 0, D3D11_MAP_WRITE_DISCARD).pData, &param, sizeof(Param));
	ImCtx::get()->Unmap(oceanParamBuffer, 0);

	ImCtx::get()->CSSetConstantBuffer({ oceanParamBuffer }, 1);
	ImCtx::get()->CSSetSRV({ gaussTexture }, 0);
	ImCtx::get()->CSSetUAV({ tildeh0k->getMip(0),waveData->getMip(0) }, 0);

	ImCtx::get()->BindShader(phillipSpectrumShader);

	ImCtx::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ImCtx::get()->CSSetSRV({ tildeh0k }, 0);
	ImCtx::get()->CSSetUAV({ tildeh0->getMip(0) }, 0);

	ImCtx::get()->BindShader(conjugatedCalcCS);

	ImCtx::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);
}

inline void Ocean::IFFT(ComputeTexture* const cTexture) const
{
	ImCtx::get()->CSSetUAV({ tempTexture->getMip(0) }, 0);
	ImCtx::get()->CSSetSRV({ cTexture }, 0);

	ImCtx::get()->BindShader(ifftShader);
	ImCtx::get()->Dispatch(param.mapResolution, 1u, 1u);

	ImCtx::get()->CSSetUAV({ cTexture->getMip(0) }, 0);
	ImCtx::get()->CSSetSRV({ tempTexture }, 0);

	ImCtx::get()->BindShader(ifftShader);
	ImCtx::get()->Dispatch(param.mapResolution, 1u, 1u);

	ImCtx::get()->BindShader(permutationCS);
	ImCtx::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);
}

inline void Ocean::update() const
{
	ImCtx::get()->CSSetConstantBuffer({ oceanParamBuffer }, 1);

	ImCtx::get()->CSSetSRV({ tildeh0,waveData }, 0);
	ImCtx::get()->CSSetUAV({ Dy->getMip(0),Dx->getMip(0),Dz->getMip(0),Dyx->getMip(0),Dyz->getMip(0),Dxx->getMip(0),Dzz->getMip(0),Dxz->getMip(0) }, 0);

	ImCtx::get()->BindShader(displacementShader);

	ImCtx::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	IFFT(Dy);
	IFFT(Dx);
	IFFT(Dz);
	IFFT(Dyx);
	IFFT(Dyz);
	IFFT(Dxx);
	IFFT(Dzz);
	IFFT(Dxz);

	ImCtx::get()->CSSetSRV({ Dy,Dx,Dz,Dyx,Dyz,Dxx,Dzz,Dxz }, 0);
	ImCtx::get()->CSSetUAV({ Dxyz->getMip(0),normalJacobian->getMip(0) }, 0);

	ImCtx::get()->BindShader(waveMergeCS);
	ImCtx::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);
}

inline void Ocean::render() const
{
	ImCtx::get()->IASetInputLayout(inputLayout.Get());
	ImCtx::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	ImCtx::get()->IASetVertexBuffer({ patchVertexBuffer }, { sizeof(Vertex) }, { 0 });

	ImCtx::get()->BindShader(oceanVShader);
	ImCtx::get()->BindShader(oceanHShader);
	ImCtx::get()->BindShader(oceanDShader);
	ImCtx::get()->BindShader(oceanPShader);

	ImCtx::get()->DSSetSRV({ Dxyz }, 0);
	ImCtx::get()->PSSetSRV({ normalJacobian }, 0);

	ImCtx::get()->PSSetSampler({ States::linearWrapSampler }, 0);
	ImCtx::get()->DSSetSampler({ States::linearWrapSampler }, 0);

	ImCtx::get()->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);

	ImCtx::get()->Draw(4 * 256 * 256, 0);

	ImCtx::get()->HSUnbindShader();
	ImCtx::get()->DSUnbindShader();
}

