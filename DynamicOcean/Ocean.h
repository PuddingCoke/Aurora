#pragma once

#include<Aurora/Renderer.h>
#include<Aurora/DX/Resource/Buffer.h>
#include<Aurora/Shader.h>
#include<Aurora/ResourceTexture.h>
#include<Aurora/States.h>
#include<Aurora/Camera.h>
#include<Aurora/ComputeTexture.h>
#include<Aurora/ComputeBuffer.h>
#include<Aurora/RenderAPI.h>

#include<memory>

class Ocean
{
public:

	Ocean(const unsigned int& mapResolution, const float& mapLength, const DirectX::XMFLOAT2& wind, const float& phillipParam);

	~Ocean();

	void update() const;

	void render() const;

private:

	ComputeTexture* const tildeh0k;

	ComputeTexture* const tildeh0mkconj;

	ComputeTexture* const tempTexture;

	ComputeTexture* const displacementY;

	ComputeTexture* const displacementX;

	ComputeTexture* const displacementZ;

	ComputeTexture* const slopeXTexture;

	ComputeTexture* const slopeZTexture;

	ComputeTexture* const displacementXYZ;

	ComputeTexture* const normalTexture;

	ResourceTexture* const gaussTexture;

	Buffer* patchVertexBuffer;

	Buffer* const oceanParamBuffer;

	Shader* const phillipSpectrumShader;

	Shader* const displacementShader;

	Shader* const ifftShader;

	Shader* const signCorrectionShader;

	Shader* const oceanVShader;

	Shader* const oceanHShader;

	Shader* const oceanDShader;

	Shader* const oceanPShader;

	ComPtr<ID3D11InputLayout> inputLayout;

	static constexpr unsigned int patchSize = 32;

	struct Param
	{
		unsigned int mapResolution;
		float mapLength;
		DirectX::XMFLOAT2 wind;
		float amplitude;
		float gravity;
		unsigned int log2MapResolution;
		float inverseTileSize;
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
	param{ mapResolution, mapLength, wind, phillipParam, 9.81f,(unsigned int)log2(mapResolution),0.f },
	tildeh0k(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	tildeh0mkconj(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	displacementY(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	displacementX(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	displacementZ(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	slopeXTexture(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	slopeZTexture(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	tempTexture(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT)),
	displacementXYZ(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32B32A32_FLOAT)),
	normalTexture(new ComputeTexture(mapResolution, mapResolution, DXGI_FORMAT_R32G32B32A32_FLOAT)),
	gaussTexture(new ResourceTexture(mapResolution, mapResolution, Texture2D::TextureType::Gauss)),
	phillipSpectrumShader(new Shader("PhillipsSpectrum.hlsl", ShaderType::Compute)),
	displacementShader(new Shader("Displacement.hlsl", ShaderType::Compute)),
	ifftShader(new Shader("IFFT.hlsl", ShaderType::Compute, { {"SIMRES","1024"} })),
	signCorrectionShader(new Shader("SignCorrection.hlsl", ShaderType::Compute)),
	oceanVShader(new Shader("OceanVShader.hlsl", ShaderType::Vertex)),
	oceanHShader(new Shader("OceanHShader.hlsl", ShaderType::Hull)),
	oceanDShader(new Shader("OceanDShader.hlsl", ShaderType::Domain)),
	oceanPShader(new Shader("OceanPShader.hlsl", ShaderType::Pixel))
{

	{
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[2] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		Renderer::device->CreateInputLayout(inputLayoutDesc, ARRAYSIZE(inputLayoutDesc), SHADERDATA(oceanVShader), inputLayout.ReleaseAndGetAddressOf());
	}

	{
		std::vector<Vertex> vertices;

		auto getVertexAt = [this](const unsigned int& x, const unsigned int& z)
		{
			const float xPos = ((float)x - (float)patchSize / 2.f) * (float)param.mapLength / (float)patchSize;
			const float zPos = ((float)z - (float)patchSize / 2.f) * (float)param.mapLength / (float)patchSize;
			const float u = (float)x / (float)patchSize;
			const float v = (float)z / (float)patchSize;
			return Vertex{ {xPos,0.f,zPos},{u,v} };
		};

		for (unsigned int z = 0; z < patchSize - 1u; z++)
		{
			for (unsigned int x = 0; x < patchSize - 1u; x++)
			{
				vertices.push_back(getVertexAt(x, z));
				vertices.push_back(getVertexAt(x, z + 1u));
				vertices.push_back(getVertexAt(x + 1u, z + 1u));
				vertices.push_back(getVertexAt(x + 1u, z));
			}
		}

		patchVertexBuffer = new Buffer(sizeof(Vertex) * vertices.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE, vertices.data());
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
	delete signCorrectionShader;
	delete oceanVShader;
	delete oceanHShader;
	delete oceanDShader;
	delete oceanPShader;

	delete tildeh0k;
	delete tildeh0mkconj;
	delete gaussTexture;
	delete displacementY;
	delete displacementX;
	delete displacementZ;
	delete slopeXTexture;
	delete slopeZTexture;
	delete tempTexture;
	delete displacementXYZ;
	delete normalTexture;
}

inline void Ocean::calculatePhillipTexture() const
{
	memcpy(oceanParamBuffer->map(0).pData, &param, sizeof(Param));
	oceanParamBuffer->unmap(0);

	RenderAPI::get()->CSSetBuffer({ oceanParamBuffer }, 1);
	RenderAPI::get()->CSSetSRV({ gaussTexture }, 0);
	RenderAPI::get()->CSSetUAV({ tildeh0k,tildeh0mkconj }, 0);

	phillipSpectrumShader->use();

	RenderAPI::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);
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
}

inline void Ocean::update() const
{
	RenderAPI::get()->CSSetSRV({ tildeh0k,tildeh0mkconj }, 0);
	RenderAPI::get()->CSSetUAV({ displacementY,displacementX,displacementZ,slopeXTexture,slopeZTexture }, 0);

	RenderAPI::get()->CSSetBuffer({ oceanParamBuffer }, 1);

	displacementShader->use();

	RenderAPI::get()->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	IFFT(displacementY);
	IFFT(displacementX);
	IFFT(displacementZ);
	IFFT(slopeXTexture);
	IFFT(slopeZTexture);

	RenderAPI::get()->CSSetSRV({ displacementY,displacementX,displacementZ,slopeXTexture,slopeZTexture }, 0);
	RenderAPI::get()->CSSetUAV({ displacementXYZ,normalTexture }, 0);

	signCorrectionShader->use();
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

	RenderAPI::get()->DSSetSRV({ displacementXYZ }, 0);
	RenderAPI::get()->PSSetSRV({ normalTexture }, 0);

	RenderAPI::get()->PSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);
	RenderAPI::get()->DSSetSampler(States::get()->linearClampSampler.GetAddressOf(), 0, 1);

	RenderAPI::get()->PSSetBuffer({ Camera::getViewBuffer() }, 1);

	RenderAPI::get()->Draw(4 * (patchSize - 1u) * (patchSize - 1u), 0u);
}

