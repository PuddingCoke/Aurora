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

	IBuffer* indexBuffer;

	int indexCount;

	Buffer* vertexBuffer;

	DBuffer* oceanParamBuffer;

	Shader* phillipSpectrumShader;

	Texture2D* phillipTexture;

	ComPtr<ID3D11UnorderedAccessView> phillipUAV;

private:

	//风或者引力常量变化都要重新计算phillipTexture
	void calculatePhillipTexture();

	struct Param
	{
		unsigned int mapResolution;
		float mapLength;
		DirectX::XMFLOAT2 wind;
		float phillipParam;
		float gravity;
		DirectX::XMFLOAT2 v0;
	}param;

};

inline Ocean::Ocean(const unsigned int& mapResolution, const float& mapLength, const DirectX::XMFLOAT2& wind, const float& phillipParam) :
	vertexBuffer(nullptr), indexBuffer(nullptr),
	oceanParamBuffer(DBuffer::create(sizeof(Param), D3D11_BIND_CONSTANT_BUFFER)),
	param{ mapResolution, mapLength, wind, phillipParam, 9.81f },
	phillipTexture(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	phillipSpectrumShader(Shader::fromFile("PhillipsSpectrum.hlsl", ShaderType::Compute))
{
	//初始化顶点索引缓冲
	{
		unsigned int* indices = new unsigned int[(mapResolution - 1u) * (mapResolution - 1u) * 6u];

		for (unsigned int y = 0; y < mapResolution - 1u; y++) {
			for (unsigned int x = 0; x < mapResolution - 1u; x++) {
				unsigned int index = y * mapResolution + x;
				indices[indexCount++] = index;
				indices[indexCount++] = index + mapResolution;
				indices[indexCount++] = index + mapResolution + 1u;
				indices[indexCount++] = index;
				indices[indexCount++] = index + mapResolution + 1u;
				indices[indexCount++] = index + 1u;
			}
		}

		indexBuffer = IBuffer::create(sizeof(unsigned int) * (mapResolution - 1u) * (mapResolution - 1u) * 6u, D3D11_BIND_INDEX_BUFFER, indices);

		delete[] indices;
	}

	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = phillipTexture->getFormat();
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateUnorderedAccessView(phillipTexture->getTexture2D(), &uavDesc, phillipUAV.ReleaseAndGetAddressOf());
	}

	calculatePhillipTexture();
}

inline Ocean::~Ocean()
{
	if (vertexBuffer)
	{
		delete vertexBuffer;
	}
	if (oceanParamBuffer)
	{
		delete oceanParamBuffer;
	}
	delete indexBuffer;
	delete phillipSpectrumShader;
	delete phillipTexture;
}

inline void Ocean::calculatePhillipTexture()
{
	//std::cout << param.gravity << "\n" << param.mapLength << "\n" << param.mapResolution << "\n" << param.phillipParam << "\n" << param.wind.x << " " << param.wind.y << "\n";

	memcpy(oceanParamBuffer->map(0).pData, &param, sizeof(Param));
	oceanParamBuffer->unmap(0);

	oceanParamBuffer->CSSetBuffer(1);

	Renderer::context->CSSetUnorderedAccessViews(0, 1, phillipUAV.GetAddressOf(), nullptr);

	phillipSpectrumShader->use();

	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ID3D11UnorderedAccessView* nullUAV = nullptr;

	Renderer::context->CSSetUnorderedAccessViews(0, 1, &nullUAV, nullptr);
}
