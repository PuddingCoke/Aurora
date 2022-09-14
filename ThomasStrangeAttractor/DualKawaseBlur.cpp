#include "DualKawaseBlur.h"

DualKawaseBlur::DualKawaseBlur(const unsigned int& width, const unsigned int& height, const float& pixelOffset) :
	EffectBase(width, height), pixelOffset(pixelOffset)
{
	compileShaders();

	bloomTexture = RenderTexture::create(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT);

	{
		DirectX::XMFLOAT4 halfPixel = DirectX::XMFLOAT4(0.5f / width, 0.5f / height, 0.f, 0.f);

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(DirectX::XMFLOAT4);
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = &halfPixel;

		Renderer::device->CreateBuffer(&bd, &data, screenHalfPixelBuffer.ReleaseAndGetAddressOf());
	}

	for (int i = 0; i < iterations; i++)
	{
		resolutions[i] = DirectX::XMUINT2(width >> (i + 1), height >> (i + 1));
		renderTextures[i] = RenderTexture::create(resolutions[i].x, resolutions[i].y, DXGI_FORMAT_R16G16B16A16_FLOAT);

		DirectX::XMFLOAT4 halfPixel = DirectX::XMFLOAT4(0.5f / resolutions[i].x, 0.5f / resolutions[i].y, 0.f, 0.f);

		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(DirectX::XMFLOAT4);
		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = &halfPixel;

		Renderer::device->CreateBuffer(&bd, &data, halfPixelBuffer[i].ReleaseAndGetAddressOf());
	}

	//初始化offsetBuffer
	{
		D3D11_BUFFER_DESC bd = {};
		bd.ByteWidth = sizeof(DirectX::XMFLOAT4);
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		DirectX::XMFLOAT4 offset = DirectX::XMFLOAT4(pixelOffset, 0.f, 0.f, 0.f);

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = &offset;

		Renderer::device->CreateBuffer(&bd, &data, offsetBuffer.ReleaseAndGetAddressOf());
	}
}

DualKawaseBlur::~DualKawaseBlur()
{
	for (int i = 0; i < iterations; i++)
	{
		delete renderTextures[i];
	}

	delete downSampleShader;
	delete upSampleShader;
	delete bloomTexture;
	delete bloomFinal;
}

const float& DualKawaseBlur::getPixelOffset() const
{
	return pixelOffset;
}

void DualKawaseBlur::setPixelOffset(const float& pixelOffset)
{
	this->pixelOffset = pixelOffset;
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(offsetBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &pixelOffset, sizeof(pixelOffset));
	Renderer::context->Unmap(offsetBuffer.Get(), 0);
}

Texture2D* DualKawaseBlur::process(Texture2D* const texture2D) const
{
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };

	Renderer::setTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Renderer::setBlendState(StateCommon::addtiveBlend.Get());
	Renderer::setSampler(0, StateCommon::defLinearSampler.Get());
	Renderer::context->PSSetConstantBuffers(1, 1, offsetBuffer.GetAddressOf());

	Shader::displayVShader->use();

	Renderer::context->PSSetConstantBuffers(2, 1, halfPixelBuffer[0].GetAddressOf());
	Renderer::setViewport(resolutions[0].x, resolutions[0].y);
	renderTextures[0]->clearRTV(DirectX::Colors::Black);
	renderTextures[0]->setRTV();
	texture2D->setSRV(0);

	Shader::displayVShader->use();
	downSampleShader->use();
	Renderer::drawQuad();

	for (int i = 1; i < iterations; i++)
	{
		Renderer::context->PSSetConstantBuffers(2, 1, halfPixelBuffer[i].GetAddressOf());
		Renderer::setViewport(resolutions[i].x, resolutions[i].y);
		renderTextures[i]->clearRTV(DirectX::Colors::Black);
		renderTextures[i]->setRTV();
		renderTextures[i - 1]->getTexture()->setSRV();

		downSampleShader->use();
		Renderer::drawQuad();
	}

	Renderer::context->PSSetShaderResources(0, 1, nullSRV);

	//升采样
	for (int i = iterations - 2; i > -1; i--)
	{
		Renderer::context->PSSetConstantBuffers(2, 1, halfPixelBuffer[i].GetAddressOf());
		Renderer::setViewport(resolutions[i].x, resolutions[i].y);
		renderTextures[i]->clearRTV(DirectX::Colors::Black);
		renderTextures[i]->setRTV();
		renderTextures[i + 1]->getTexture()->setSRV();

		upSampleShader->use();
		Renderer::drawQuad();
	}

	Renderer::context->PSSetConstantBuffers(2, 1, screenHalfPixelBuffer.GetAddressOf());
	Renderer::setViewport(width, height);
	bloomTexture->clearRTV(DirectX::Colors::Black);
	bloomTexture->setRTV();

	renderTextures[0]->getTexture()->setSRV();
	upSampleShader->use();
	Renderer::drawQuad();

	texture2D->setSRV();
	Shader::displayPShader->use();
	Renderer::drawQuad();

	outputRTV->clearRTV(DirectX::Colors::Black);
	outputRTV->setRTV();

	bloomTexture->getTexture()->setSRV(0);

	bloomFinal->use();
	Renderer::drawQuad();

	Renderer::context->PSSetShaderResources(0, 1, nullSRV);

	return outputRTV->getTexture();
}

void DualKawaseBlur::compileShaders()
{
	downSampleShader = Shader::fromFile("DownSampleShader.hlsl", ShaderType::Pixel);
	upSampleShader = Shader::fromFile("UpSampleShader.hlsl", ShaderType::Pixel);
	bloomFinal = Shader::fromFile("BloomFinal.hlsl", ShaderType::Pixel);
}
