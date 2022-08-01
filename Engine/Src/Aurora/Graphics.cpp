#include<Aurora/Graphics.h>

ID3D11Texture2D* Graphics::backBuffer = nullptr;

ComPtr<ID3D11Texture2D> Graphics::encodeTexture;

ComPtr<ID3D11Device4> Graphics::device;

ComPtr<ID3D11DeviceContext4> Graphics::context;

ComPtr<ID3D11Buffer> Graphics::cBufferProj;

ComPtr<ID3D11Buffer> Graphics::cBufferView;

ComPtr<ID3D11Buffer> Graphics::cBufferDeltaTimes;

ComPtr<ID3D11Debug> Graphics::d3dDebug;

Graphics::GPUDeltaTimes Graphics::gpuDeltaTimes;

ComPtr<ID3D11RenderTargetView> Graphics::defaultTargetView;

float Graphics::deltaTime = 0;

float Graphics::sTime = 0;

float Graphics::frameDuration = 0;

int Graphics::frameCount = 0;

float Graphics::framePerSec = 0;

int Graphics::width = 0;

int Graphics::height = 0;

float Graphics::aspectRatio = 0;

unsigned int Graphics::msaaLevel = 0;

D3D11_VIEWPORT Graphics::vp = {};

void Graphics::setDefRTV(ID3D11DepthStencilView* const view)
{
	Graphics::context->OMSetRenderTargets(1, Graphics::defaultTargetView.GetAddressOf(), view);
}

void Graphics::clearDefRTV(const float color[4])
{
	Graphics::context->ClearRenderTargetView(Graphics::defaultTargetView.Get(), color);
}

const float& Graphics::getDeltaTime()
{
	return deltaTime;
}

const float& Graphics::getSTime()
{
	return sTime;
}

const float& Graphics::getFPS()
{
	frameDuration += deltaTime;
	frameCount++;
	if (frameDuration > .1f)
	{
		framePerSec = frameCount / frameDuration;
		frameCount = 0;
		frameDuration = 0;
	}
	return framePerSec;
}

const float& Graphics::getAspectRatio()
{
	return aspectRatio;
}

void Graphics::setView(const DirectX::XMMATRIX& view)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Graphics::context->Map(cBufferView.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &view, sizeof(DirectX::XMMATRIX));
	Graphics::context->Unmap(cBufferView.Get(), 0);
}

const int& Graphics::getWidth()
{
	return width;
}

const int& Graphics::getHeight()
{
	return height;
}

unsigned int& Graphics::getMSAALevel()
{
	return msaaLevel;
}

void Graphics::setViewport(const float& vWidth, const float& vHeight)
{
	vp.Width = vWidth;
	vp.Height = vHeight;
	Graphics::context->RSSetViewports(1, &vp);
}

void Graphics::setPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology)
{
	Graphics::context->IASetPrimitiveTopology(topology);
}

void Graphics::setBlendState(ID3D11BlendState* const blendState)
{
	Graphics::context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
}

void Graphics::setProj(const DirectX::XMMATRIX& proj)
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Graphics::context->Map(cBufferProj.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &proj, sizeof(DirectX::XMMATRIX));
	Graphics::context->Unmap(cBufferProj.Get(), 0);
}

void Graphics::updateGPUDeltaTimes()
{
	gpuDeltaTimes.deltaTime = Graphics::deltaTime;
	gpuDeltaTimes.sTime = Graphics::sTime;
	//v2=
	//v3=


	D3D11_MAPPED_SUBRESOURCE mappedData;
	Graphics::context->Map(cBufferDeltaTimes.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &gpuDeltaTimes, sizeof(GPUDeltaTimes));
	Graphics::context->Unmap(cBufferDeltaTimes.Get(), 0);
}
