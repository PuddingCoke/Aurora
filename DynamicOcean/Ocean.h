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

	Texture2D* const slopeXTexture;

	Texture2D* const slopeZTexture;

	Texture2D* const displacementXYZ;

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

	ComPtr<ID3D11UnorderedAccessView> slopeXUAV;

	ComPtr<ID3D11UnorderedAccessView> slopeZUAV;

	ComPtr<ID3D11UnorderedAccessView> tempTextureUAV;

	ComPtr<ID3D11UnorderedAccessView> displacementXYZUAV;

	ComPtr<ID3D11UnorderedAccessView> normalTextureUAV;

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
	slopeXTexture(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	slopeZTexture(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	tempTexture(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	displacementXYZ(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32B32A32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
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
		Renderer::device->CreateUnorderedAccessView(slopeXTexture->getTexture2D(), &uavDesc, slopeXUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(slopeZTexture->getTexture2D(), &uavDesc, slopeZUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(tempTexture->getTexture2D(), &uavDesc, tempTextureUAV.ReleaseAndGetAddressOf());

		uavDesc.Format = normalTexture->getFormat();

		Renderer::device->CreateUnorderedAccessView(displacementXYZ->getTexture2D(), &uavDesc, displacementXYZUAV.ReleaseAndGetAddressOf());
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

	{
		uav = tempTextureUAV.Get();
		displacementY->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		uav = displacementYUAV.Get();
		tempTexture->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}

	{
		uav = tempTextureUAV.Get();
		displacementX->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		uav = displacementXUAV.Get();
		tempTexture->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}

	{
		uav = tempTextureUAV.Get();
		displacementZ->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		uav = displacementZUAV.Get();
		 tempTexture->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}

	{
		uav = tempTextureUAV.Get();
		slopeXTexture->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		uav = slopeXUAV.Get();
		tempTexture->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}

	{
		uav = tempTextureUAV.Get();
		slopeZTexture->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);

		uav = slopeZUAV.Get();
		tempTexture->CSSetSRV();

		Renderer::context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		ifftShader->use();
		Renderer::context->Dispatch(param.mapResolution, 1u, 1u);

		Renderer::context->CSSetUnorderedAccessViews(0, 1, nullUAV, nullptr);
		Renderer::context->CSSetShaderResources(0, 1, nullSRV);
	}

}

inline void Ocean::calcDisplacement() const
{
	tildeh0k->CSSetSRV(0);
	tildeh0mkconj->CSSetSRV(1);

	ID3D11UnorderedAccessView* const uav[5] = { displacementYUAV.Get(),displacementXUAV.Get(),displacementZUAV.Get(),slopeXUAV.Get(),slopeZUAV.Get() };
	Renderer::context->CSSetUnorderedAccessViews(0, 5, uav, nullptr);

	oceanParamBuffer->CSSetBuffer(1);

	displacementShader->use();

	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ID3D11ShaderResourceView* const nullSRV[2] = { nullptr,nullptr };
	Renderer::context->CSSetShaderResources(0, 2, nullSRV);
	ID3D11UnorderedAccessView* const nullUAV[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
	Renderer::context->CSSetUnorderedAccessViews(0, 5, nullUAV, nullptr);

	calculateIFFT();

	{
		displacementY->CSSetSRV(0);
		displacementX->CSSetSRV(1);
		displacementZ->CSSetSRV(2);
		slopeXTexture->CSSetSRV(3);
		slopeZTexture->CSSetSRV(4);

		ID3D11UnorderedAccessView* const uavs[2] = { displacementXYZUAV.Get(),normalTextureUAV.Get() };

		Renderer::context->CSSetUnorderedAccessViews(0, 2, uavs, nullptr);

		signCorrectionShader->use();
		Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

		ID3D11ShaderResourceView* const nullSRVS[5] = { nullptr,nullptr,nullptr,nullptr,nullptr };
		ID3D11UnorderedAccessView* const nullUAVS[2] = { nullptr,nullptr };

		Renderer::context->CSSetShaderResources(0, 5, nullSRVS);
		Renderer::context->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
	}
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

