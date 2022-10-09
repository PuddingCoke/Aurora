#pragma once

#include<Aurora/Renderer.h>
#include<Aurora/IBuffer.h>
#include<Aurora/DBuffer.h>
#include<Aurora/Shader.h>
#include<Aurora/Texture2D.h>

#include<memory>

class Ocean
{
public:

	Ocean(const unsigned int& mapResolution, const float& mapLength, const DirectX::XMFLOAT2& wind, const float& phillipParam);

	~Ocean();

	Texture2D* const tildeh0k;

	Texture2D* const tildeh0mkconj;

	Texture2D* const tempTexture;

	Texture2D* const gaussTexture;

	Texture2D* const displacementY;

	Texture2D* const displacementX;

	Texture2D* const displacementZ;

	Texture2D* const normalTexture;

	void calcDisplacement() const;

	void render() const;

private:

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

	ComPtr<ID3D11UnorderedAccessView> tildeh0kUAV;

	ComPtr<ID3D11UnorderedAccessView> tildeh0mkconjUAV;

	ComPtr<ID3D11UnorderedAccessView> displacementYUAV;

	ComPtr<ID3D11UnorderedAccessView> displacementXUAV;

	ComPtr<ID3D11UnorderedAccessView> displacementZUAV;

	ComPtr<ID3D11UnorderedAccessView> tempTextureUAV;

	ComPtr<ID3D11UnorderedAccessView> normalTextureUAV;

	ComPtr<ID3D11InputLayout> inputLayout;

	//128x128 per grid
	static constexpr unsigned int patchSize = 16;

	struct Param
	{
		unsigned int mapResolution;
		float mapLength;
		DirectX::XMFLOAT2 wind;
		float amplitude;
		float gravity;
		unsigned int log2MapResolution;
		float v0;
	}param;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
	};

	//风或者引力常量变化都要重新计算phillipTexture
	void calculatePhillipTexture() const;

	void calculateIFFT() const;

};

inline Ocean::Ocean(const unsigned int& mapResolution, const float& mapLength, const DirectX::XMFLOAT2& wind, const float& phillipParam) :
	oceanParamBuffer(DBuffer::create(sizeof(Param), D3D11_BIND_CONSTANT_BUFFER)),
	param{ mapResolution, mapLength, wind, phillipParam, 9.81f,(unsigned int)log2(mapResolution),0.f },
	tildeh0k(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	tildeh0mkconj(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	displacementY(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	displacementX(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	displacementZ(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	tempTexture(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	normalTexture(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	gaussTexture(Texture2D::createGauss(mapResolution, mapResolution)),
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
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = tildeh0k->getFormat();
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateUnorderedAccessView(tildeh0k->getTexture2D(), &uavDesc, tildeh0kUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(tildeh0mkconj->getTexture2D(), &uavDesc, tildeh0mkconjUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(displacementY->getTexture2D(), &uavDesc, displacementYUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(displacementX->getTexture2D(), &uavDesc, displacementXUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(displacementZ->getTexture2D(), &uavDesc, displacementZUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(tempTexture->getTexture2D(), &uavDesc, tempTextureUAV.ReleaseAndGetAddressOf());

		uavDesc.Format = normalTexture->getFormat();

		Renderer::device->CreateUnorderedAccessView(normalTexture->getTexture2D(), &uavDesc, normalTextureUAV.ReleaseAndGetAddressOf());
	}

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
	delete tempTexture;
	delete normalTexture;
}

inline void Ocean::calculatePhillipTexture() const
{
	memcpy(oceanParamBuffer->map(0).pData, &param, sizeof(Param));
	oceanParamBuffer->unmap(0);

	ID3D11ShaderResourceView* const srv = gaussTexture->getSRV();

	Renderer::context->CSSetShaderResources(0, 1, &srv);

	oceanParamBuffer->CSSetBuffer(1);

	ID3D11UnorderedAccessView* const uav[2] = { tildeh0kUAV.Get(),tildeh0mkconjUAV.Get() };

	Renderer::context->CSSetUnorderedAccessViews(0, 2, uav, nullptr);

	phillipSpectrumShader->use();

	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ID3D11UnorderedAccessView* const nullUAV[2] = { nullptr,nullptr };

	Renderer::context->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
}

inline void Ocean::calculateIFFT() const
{
	ID3D11UnorderedAccessView* const nullUAV[1] = { nullptr };
	ID3D11ShaderResourceView* const nullSRV[1] = { nullptr };

	ID3D11UnorderedAccessView* uav;
	ID3D11ShaderResourceView* srv;

	{
		uav = tempTextureUAV.Get();
		srv = displacementY->getSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, &srv);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		uav = displacementYUAV.Get();
		srv = tempTexture->getSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, &srv);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}

	{
		uav = tempTextureUAV.Get();
		srv = displacementX->getSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, &srv);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		uav = displacementXUAV.Get();
		srv = tempTexture->getSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, &srv);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}

	{
		uav = tempTextureUAV.Get();
		srv = displacementZ->getSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, &srv);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		uav = displacementZUAV.Get();
		srv = tempTexture->getSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, &srv);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}
}

inline void Ocean::calcDisplacement() const
{
	ID3D11UnorderedAccessView* const uav[3] = { displacementYUAV.Get(),displacementXUAV.Get(),displacementZUAV.Get() };
	Renderer::context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);
	ID3D11ShaderResourceView* const srv[2] = { tildeh0k->getSRV(),tildeh0mkconj->getSRV() };
	Renderer::context->CSSetShaderResources(0, 2, srv);

	oceanParamBuffer->CSSetBuffer(1);

	displacementShader->use();

	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ID3D11ShaderResourceView* const nullSRV[2] = { nullptr,nullptr };
	Renderer::context->CSSetShaderResources(0, 2, nullSRV);
	ID3D11UnorderedAccessView* const nullUAV[3] = { nullptr,nullptr,nullptr };
	Renderer::context->CSSetUnorderedAccessViews(0, 3, nullUAV, nullptr);

	calculateIFFT();

	Renderer::context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);
	signCorrectionShader->use();
	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	Renderer::context->CSSetUnorderedAccessViews(0, 3, nullUAV, nullptr);
}

inline void Ocean::render() const
{
	ID3D11ShaderResourceView* srv = displacementY->getSRV();

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

	Renderer::context->DSSetShaderResources(0, 1, &srv);
	Renderer::context->DSSetSamplers(0, 1, States::linearClampSampler.GetAddressOf());

	Renderer::draw(4 * (patchSize - 1u) * (patchSize - 1u), 0u);

	ID3D11ShaderResourceView* nullSRV = nullptr;

	Renderer::context->DSSetShaderResources(0, 1, &nullSRV);
}

