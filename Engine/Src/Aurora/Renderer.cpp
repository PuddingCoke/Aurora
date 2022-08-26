#include<Aurora/Renderer.h>

ComPtr<ID3D11Device4> Renderer::device;

ComPtr<ID3D11DeviceContext4> Renderer::context;

D3D11_VIEWPORT Renderer::vp = { 0.f,0.f,0.f,0.f,0.f,1.f };

ID3D11Texture2D* Renderer::backBuffer = nullptr;

ComPtr<ID3D11RenderTargetView> Renderer::defaultTargetView;

void Renderer::setViewport(const float& vWidth, const float& vHeight)
{
	vp.Width = vWidth;
	vp.Height = vHeight;
	context->RSSetViewports(1, &vp);
}

void Renderer::setTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology)
{
	context->IASetPrimitiveTopology(topology);
}

void Renderer::setBlendState(ID3D11BlendState* const blendState)
{
	context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
}

void Renderer::setSampler(const unsigned int& slot, ID3D11SamplerState* const state)
{
	context->PSSetSamplers(slot, 1, &state);
}

void Renderer::setDefRTV(ID3D11DepthStencilView* const view)
{
	context->OMSetRenderTargets(1, defaultTargetView.GetAddressOf(), view);
}

void Renderer::clearDefRTV(const float* color)
{
	context->ClearRenderTargetView(defaultTargetView.Get(), color);
}