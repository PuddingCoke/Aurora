#pragma once

#include<Aurora/Renderer.h>
#include<Aurora/IBuffer.h>
#include<Aurora/DBuffer.h>
#include<Aurora/Shader.h>
#include<Aurora/Texture2D.h>
#include<Aurora/States.h>
#include<Aurora/Camera.h>
#include<Aurora/ComputeTexture.h>

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

	Texture2D* const gaussTexture;

	IBuffer* patchVertexBuffer;

	DBuffer* const oceanParamBuffer;

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
	oceanParamBuffer(DBuffer::create(sizeof(Param), D3D11_BIND_CONSTANT_BUFFER)),
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
	gaussTexture(new Texture2D(mapResolution, mapResolution, Texture2D::TextureType::Gauss)),
	phillipSpectrumShader(Shader::fromFile("PhillipsSpectrum.hlsl", ShaderType::Compute)),
	displacementShader(Shader::fromFile("Displacement.hlsl", ShaderType::Compute)),
	ifftShader(Shader::fromFile("IFFT.hlsl", ShaderType::Compute)),
	signCorrectionShader(Shader::fromFile("SignCorrection.hlsl", ShaderType::Compute)),
	oceanVShader(Shader::fromFile("OceanVShader.hlsl", ShaderType::Vertex)),
	oceanHShader(Shader::fromFile("OceanHShader.hlsl", ShaderType::Hull)),
	oceanDShader(Shader::fromFile("OceanDShader.hlsl", ShaderType::Domain)),
	oceanPShader(Shader::fromFile("OceanPShader.hlsl", ShaderType::Pixel))
{

	{
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[2] =
		{
			{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
			{"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0}
		};

		Renderer::device->CreateInputLayout(inputLayoutDesc, ARRAYSIZE(inputLayoutDesc), oceanVShader->shaderBlob->GetBufferPointer(),
			oceanVShader->shaderBlob->GetBufferSize(), inputLayout.ReleaseAndGetAddressOf());
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

		patchVertexBuffer = IBuffer::create(sizeof(Vertex) * vertices.size(), D3D11_BIND_VERTEX_BUFFER, vertices.data());
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

	gaussTexture->CSSetSRV();

	oceanParamBuffer->CSSetBuffer(1);

	tildeh0k->CSSetUAV(0);
	tildeh0mkconj->CSSetUAV(1);

	phillipSpectrumShader->use();

	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ID3D11UnorderedAccessView* const nullUAV[2] = { nullptr,nullptr };

	Renderer::context->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
}

inline void Ocean::IFFT(ComputeTexture* const cTexture) const
{
	ID3D11UnorderedAccessView* const nullUAV[1] = { nullptr };
	ID3D11ShaderResourceView* const nullSRV[1] = { nullptr };

	{
		tempTexture->CSSetUAV(0);
		cTexture->CSSetSRV();

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		cTexture->CSSetUAV(0);
		tempTexture->CSSetSRV();

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}
}

inline void Ocean::update() const
{
	tildeh0k->CSSetSRV(0);
	tildeh0mkconj->CSSetSRV(1);

	displacementY->CSSetUAV(0);
	displacementX->CSSetUAV(1);
	displacementZ->CSSetUAV(2);
	slopeXTexture->CSSetUAV(3);
	slopeZTexture->CSSetUAV(4);

	oceanParamBuffer->CSSetBuffer(1);

	displacementShader->use();

	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ID3D11ShaderResourceView* const nullSRV[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
	ID3D11UnorderedAccessView* const nullUAV[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
	Renderer::context->CSSetShaderResources(0, 5, nullSRV);
	Renderer::context->CSSetUnorderedAccessViews(0, 5, nullUAV, nullptr);

	IFFT(displacementY);
	IFFT(displacementX);
	IFFT(displacementZ);
	IFFT(slopeXTexture);
	IFFT(slopeZTexture);

	displacementY->CSSetSRV(0);
	displacementX->CSSetSRV(1);
	displacementZ->CSSetSRV(2);
	slopeXTexture->CSSetSRV(3);
	slopeZTexture->CSSetSRV(4);

	displacementXYZ->CSSetUAV(0);
	normalTexture->CSSetUAV(1);

	signCorrectionShader->use();
	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	Renderer::context->CSSetShaderResources(0, 5, nullSRV);
	Renderer::context->CSSetUnorderedAccessViews(0, 5, nullUAV, nullptr);
}

inline void Ocean::render() const
{
	const unsigned int stride = sizeof(Vertex);
	const unsigned int offset = 0;

	ID3D11Buffer* const vertexBuffer = patchVertexBuffer->get();

	Renderer::context->IASetInputLayout(inputLayout.Get());
	Renderer::context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	Renderer::context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	oceanVShader->use();
	oceanHShader->use();
	oceanDShader->use();
	oceanPShader->use();

	displacementXYZ->DSSetSRV();
	Renderer::context->DSSetSamplers(0, 1, States::linearClampSampler.GetAddressOf());

	normalTexture->PSSetSRV();
	Renderer::context->PSSetSamplers(0, 1, States::linearClampSampler.GetAddressOf());

	ID3D11Buffer* const constantBuffers[1] = { Camera::getViewBuffer() };

	Renderer::context->PSSetConstantBuffers(1, 1, constantBuffers);

	Renderer::draw(4 * (patchSize - 1u) * (patchSize - 1u), 0u);

	ID3D11ShaderResourceView* nullSRV = nullptr;

	Renderer::context->DSSetShaderResources(0, 1, &nullSRV);
}

