#include<Aurora/Graphics.h>

ID3D11Texture2D* Graphics::backBuffer = nullptr;

ComPtr<ID3D11Device4> Graphics::device;

ComPtr<ID3D11DeviceContext4> Graphics::context;

ComPtr<ID3D11Buffer> Graphics::deltaTimeBuffer;

ComPtr<ID3D11Debug> Graphics::d3dDebug;

Graphics::DeltaTime Graphics::deltaTime = { 0,0,0,0 };

ComPtr<ID3D11RenderTargetView> Graphics::defaultTargetView;

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
	return deltaTime.deltaTime;
}

const float& Graphics::getSTime()
{
	return deltaTime.sTime;
}

const float& Graphics::getFPS()
{
	frameDuration += deltaTime.deltaTime;
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

void Graphics::createDeltaTimeBuffer()
{
	D3D11_BUFFER_DESC cbd = {};
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.ByteWidth = sizeof(DeltaTime);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	Graphics::device->CreateBuffer(&cbd, nullptr, Graphics::deltaTimeBuffer.ReleaseAndGetAddressOf());
	Graphics::updateDeltaTimeBuffer();
	Graphics::context->PSSetConstantBuffers(0, 1, Graphics::deltaTimeBuffer.GetAddressOf());
	Graphics::context->CSSetConstantBuffers(0, 1, Graphics::deltaTimeBuffer.GetAddressOf());
}

void Graphics::updateDeltaTimeBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Graphics::context->Map(deltaTimeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &deltaTime, sizeof(DeltaTime));
	Graphics::context->Unmap(deltaTimeBuffer.Get(), 0);
}
