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

	DBuffer* oceanParamBuffer;

	Shader* phillipSpectrumShader;

	Shader* displacementShader;

	Texture2D* tildeh0k;

	Texture2D* tildeh0mk;

	Texture2D* gaussTexture;

	Texture2D* displacementY;

	Texture2D* displacementX;

	Texture2D* displacementZ;

	ComPtr<ID3D11UnorderedAccessView> tildeh0kUAV;

	ComPtr<ID3D11UnorderedAccessView> tildeh0mkUAV;

	ComPtr<ID3D11UnorderedAccessView> displacementYUAV;

	ComPtr<ID3D11UnorderedAccessView> displacementXUAV;

	ComPtr<ID3D11UnorderedAccessView> displacementZUAV;

	void calcDisplacement();

private:

	//风或者引力常量变化都要重新计算phillipTexture
	void calculatePhillipTexture();

	struct Param
	{
		unsigned int mapResolution;
		float mapLength;
		DirectX::XMFLOAT2 wind;
		float amplitude;
		float gravity;
		DirectX::XMFLOAT2 v0;
	}param;

};

inline Ocean::Ocean(const unsigned int& mapResolution, const float& mapLength, const DirectX::XMFLOAT2& wind, const float& phillipParam) :
	oceanParamBuffer(DBuffer::create(sizeof(Param), D3D11_BIND_CONSTANT_BUFFER)),
	param{ mapResolution, mapLength, wind, phillipParam, 9.81f },
	tildeh0k(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	tildeh0mk(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	displacementY(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	displacementX(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	displacementZ(Texture2D::create(mapResolution, mapResolution, DXGI_FORMAT_R32G32_FLOAT, D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS, 0)),
	gaussTexture(Texture2D::createGauss(mapResolution, mapResolution)),
	phillipSpectrumShader(Shader::fromFile("PhillipsSpectrum.hlsl", ShaderType::Compute)),
	displacementShader(Shader::fromFile("Displacement.hlsl", ShaderType::Compute))
{
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = tildeh0k->getFormat();
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;

		Renderer::device->CreateUnorderedAccessView(tildeh0k->getTexture2D(), &uavDesc, tildeh0kUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(tildeh0mk->getTexture2D(), &uavDesc, tildeh0mkUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(displacementY->getTexture2D(), &uavDesc, displacementYUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(displacementX->getTexture2D(), &uavDesc, displacementXUAV.ReleaseAndGetAddressOf());
		Renderer::device->CreateUnorderedAccessView(displacementZ->getTexture2D(), &uavDesc, displacementZUAV.ReleaseAndGetAddressOf());
	}

	calculatePhillipTexture();
}

inline Ocean::~Ocean()
{
	delete oceanParamBuffer;

	delete phillipSpectrumShader;
	delete displacementShader;

	delete tildeh0k;
	delete tildeh0mk;
	delete gaussTexture;
	delete displacementY;
	delete displacementX;
	delete displacementZ;
}

inline void Ocean::calculatePhillipTexture()
{
	memcpy(oceanParamBuffer->map(0).pData, &param, sizeof(Param));
	oceanParamBuffer->unmap(0);

	ID3D11ShaderResourceView* srv = gaussTexture->getSRV();

	Renderer::context->CSSetShaderResources(0, 1, &srv);

	oceanParamBuffer->CSSetBuffer(1);

	ID3D11UnorderedAccessView* uav[2] = { tildeh0kUAV.Get(),tildeh0mkUAV.Get() };

	Renderer::context->CSSetUnorderedAccessViews(0, 2, uav, nullptr);

	phillipSpectrumShader->use();

	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ID3D11UnorderedAccessView* nullUAV[2] = { nullptr,nullptr };

	Renderer::context->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);
}

inline void Ocean::calcDisplacement()
{
	ID3D11UnorderedAccessView* uav[3] = { displacementYUAV.Get(),displacementXUAV.Get(),displacementZUAV.Get() };
	Renderer::context->CSSetUnorderedAccessViews(0, 3, uav, nullptr);
	ID3D11ShaderResourceView* srv[2] = { tildeh0k->getSRV(),tildeh0mk->getSRV() };
	Renderer::context->CSSetShaderResources(0, 2, srv);

	oceanParamBuffer->CSSetBuffer(1);

	displacementShader->use();

	Renderer::context->Dispatch(param.mapResolution / 32u, param.mapResolution / 32u, 1u);

	ID3D11UnorderedAccessView* nullUAV[3] = { nullptr,nullptr,nullptr };
	Renderer::context->CSSetUnorderedAccessViews(0, 3, nullUAV, nullptr);
	ID3D11ShaderResourceView* nullSRV[2] = { nullptr,nullptr };
	Renderer::context->CSSetShaderResources(0, 2, nullSRV);
}
