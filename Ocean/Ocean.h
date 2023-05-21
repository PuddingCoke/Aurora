#pragma once

#include<Aurora/Core/RenderAPI.h>
#include<Aurora/Core/States.h>
#include<Aurora/Core/Shader.h>
#include<Aurora/Core/ResourceEssential.h>

class Ocean
{
public:

	Ocean(const unsigned int& mapResolution, const float& mapLength, const DirectX::XMFLOAT2& wind, const float& phillipParam);

	~Ocean();

	void update() const;

	void render() const;

private:

	ComputeTexture* tildeh0k;

	ComputeTexture* tildeh0mkconj;

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

	Buffer* patchVertexBuffer;

	Buffer* oceanParamBuffer;

	Shader* phillipSpectrumShader;

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
	oceanParamBuffer(new Buffer(sizeof(Param), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, nullptr, D3D11_CPU_ACCESS_WRITE)),
	param{ mapResolution, mapLength, wind, phillipParam, 9.81f },
	tildeh0k(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	tildeh0mkconj(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dy(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dx(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dz(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dyx(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dyz(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dxx(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dzz(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dxz(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	tempTexture(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	Dxyz(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32B32A32_FLOAT)),
	normalJacobian(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32B32A32_FLOAT)),
	phillipSpectrumShader(new Shader("PhillipsSpectrum.hlsl", ShaderType::Compute)),
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

	normalJacobian->clear(clearValue);

	{
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[2] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		Renderer::getDevice()->CreateInputLayout(inputLayoutDesc, ARRAYSIZE(inputLayoutDesc), SHADERDATA(oceanVShader), inputLayout.ReleaseAndGetAddressOf());
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

		patchVertexBuffer = new Buffer(sizeof(Vertex) * vertices.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE, vertices.data());
	}

	{
		D3D11_RASTERIZER_DESC desc = {};
		desc.CullMode = D3D11_CULL_BACK;
		desc.FillMode = D3D11_FILL_WIREFRAME;
		
		Renderer::getDevice()->CreateRasterizerState(&desc, wireState.ReleaseAndGetAddressOf());
	}

	calculatePhillipTexture();
}

inline Ocean::~Ocean()
{
	delete oceanParamBuffer;
	delete patchVertexBuffer;

	delete phillipSpectrumShader;
	delete displacementShader;
	delete ifftShader;
	delete permutationCS;
	delete waveMergeCS;
	delete oceanVShader;
	delete oceanHShader;
	delete oceanDShader;
	delete oceanPShader;

	delete tildeh0k;
	delete tildeh0mkconj;
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
	memcpy(oceanParamBuffer->map().pData, &param, sizeof(Param));
	oceanParamBuffer->unmap();

	ResourceTexture* gaussTexture = new ResourceTexture(param.mapResolution, param.mapResolution, Texture2D::TextureType::Gauss);

	RenderAPI::get()->CSSetConstantBuffer({ oceanParamBuffer }, 1);
	RenderAPI::get()->CSSetSRV({ gaussTexture }, 0);
	RenderAPI::get()->CSSetUAV({ tildeh0k,tildeh0mkconj }, 0);

	phillipSpectrumShader->use();

	RenderAPI::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	delete gaussTexture;
}

inline void Ocean::IFFT(ComputeTexture* const cTexture) const
{
	RenderAPI::get()->CSSetUAV({ tempTexture }, 0);
	RenderAPI::get()->CSSetSRV({ cTexture }, 0);

	ifftShader->use();
	RenderAPI::get()->Dispatch(param.mapResolution, 1u, 1u);

	RenderAPI::get()->CSSetUAV({ cTexture }, 0);
	RenderAPI::get()->CSSetSRV({ tempTexture }, 0);

	ifftShader->use();
	RenderAPI::get()->Dispatch(param.mapResolution, 1u, 1u);

	permutationCS->use();
	RenderAPI::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);
}

inline void Ocean::update() const
{
	RenderAPI::get()->CSSetConstantBuffer({ oceanParamBuffer }, 1);

	RenderAPI::get()->CSSetSRV({ tildeh0k,tildeh0mkconj }, 0);
	RenderAPI::get()->CSSetUAV({ Dy,Dx,Dz,Dyx,Dyz,Dxx,Dzz,Dxz }, 0);

	displacementShader->use();

	RenderAPI::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	IFFT(Dy);
	IFFT(Dx);
	IFFT(Dz);
	IFFT(Dyx);
	IFFT(Dyz);
	IFFT(Dxx);
	IFFT(Dzz);
	IFFT(Dxz);

	RenderAPI::get()->CSSetSRV({ Dy,Dx,Dz,Dyx,Dyz,Dxx,Dzz,Dxz }, 0);
	RenderAPI::get()->CSSetUAV({ Dxyz,normalJacobian }, 0);

	waveMergeCS->use();
	RenderAPI::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);
}

inline void Ocean::render() const
{
	RenderAPI::get()->IASetInputLayout(inputLayout.Get());
	RenderAPI::get()->IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	RenderAPI::get()->IASetVertexBuffer(0, { patchVertexBuffer }, { sizeof(Vertex) }, { 0 });

	oceanVShader->use();
	oceanHShader->use();
	oceanDShader->use();
	oceanPShader->use();

	RenderAPI::get()->DSSetSRV({ Dxyz }, 0);
	RenderAPI::get()->PSSetSRV({ normalJacobian }, 0);

	RenderAPI::get()->PSSetSampler({ States::linearWrapSampler }, 0);
	RenderAPI::get()->DSSetSampler({ States::linearWrapSampler }, 0);

	RenderAPI::get()->PSSetConstantBuffer({ Camera::getViewBuffer() }, 1);

	RenderAPI::get()->Draw(4 * 256 * 256, 0);

	RenderAPI::get()->HSUnbindShader();
	RenderAPI::get()->DSUnbindShader();
}

