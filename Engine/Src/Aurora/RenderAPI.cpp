#include<Aurora/RenderAPI.h>

RenderAPI* RenderAPI::instance = nullptr;

RenderAPI::RenderAPI(const unsigned int& width, const unsigned int& height, const unsigned int& msaaLevel) :
	vp{ 0.f,0.f,0.f,0.f,0.f,1.f }
{
	if (msaaLevel == 1)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		defRenderTargetView = new RenderTargetView(Renderer::instance->backBuffer.Get(), rtvDesc);
	}
	else
	{
		D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
		msaaViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION::D3D11_RTV_DIMENSION_TEXTURE2DMS;

		defRenderTargetView = new RenderTargetView(Renderer::instance->msaaTexture.Get(), msaaViewDesc);
	}
}

RenderAPI::~RenderAPI()
{
	delete defRenderTargetView;
}

void RenderAPI::SetViewport(const float& width, const float& height)
{
	vp.Width = width;
	vp.Height = height;
	Renderer::context->RSSetViewports(1, &vp);
}

void RenderAPI::SetViewport(const unsigned int& width, const unsigned int& height)
{
	SetViewport((float)width, (float)height);
}

void RenderAPI::SetViewport(const int& width, const int& height)
{
	SetViewport((float)width, (float)height);
}

void RenderAPI::SetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology)
{
	Renderer::context->IASetPrimitiveTopology(topology);
}

void RenderAPI::SetBlendState(ID3D11BlendState* const state)
{
	Renderer::context->OMSetBlendState(state, nullptr, 0xFFFFFFFF);
}

void RenderAPI::SetRasterState(ID3D11RasterizerState* const state)
{
	Renderer::context->RSSetState(state);
}

void RenderAPI::SetDepthStencilState(ID3D11DepthStencilState* const state,const UINT& stencilRef)
{
	Renderer::context->OMSetDepthStencilState(state, stencilRef);
}

void RenderAPI::DrawQuad()
{
	Renderer::context->Draw(3, 0);
}

void RenderAPI::DrawCube()
{
	Renderer::context->Draw(36, 0);
}

void RenderAPI::Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ)
{
	Renderer::context->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void RenderAPI::Draw(const UINT& vertexCount, const UINT& startVertexLocation)
{
	Renderer::context->Draw(vertexCount, startVertexLocation);
}

void RenderAPI::DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation)
{
	Renderer::context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void RenderAPI::DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation)
{
	Renderer::context->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

RenderAPI* RenderAPI::get()
{
	return instance;
}

void RenderAPI::OMSetDefRTV(ID3D11DepthStencilView* const dsv)
{
	ResManager::get()->OMSetRTV({ defRenderTargetView }, dsv);
}

void RenderAPI::ClearDefRTV(const float* const color)
{
	defRenderTargetView->clearRTV(color);
}

void RenderAPI::OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, ID3D11DepthStencilView* const dsv)
{
	ResManager::get()->OMSetRTV(rtvs, dsv);
}

void RenderAPI::VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	ResManager::get()->VSSetSRV(srvs, slot);
}

void RenderAPI::HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	ResManager::get()->HSSetSRV(srvs, slot);
}

void RenderAPI::DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	ResManager::get()->DSSetSRV(srvs, slot);
}

void RenderAPI::GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	ResManager::get()->GSSetSRV(srvs, slot);
}

void RenderAPI::PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	ResManager::get()->PSSetSRV(srvs, slot);
}

void RenderAPI::CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot)
{
	ResManager::get()->CSSetSRV(srvs, slot);
}

void RenderAPI::CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot)
{
	ResManager::get()->CSSetUAV(uavs, slot);
}

void RenderAPI::VSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	ResManager::get()->VSSetBuffer(buffers, slot);
}

void RenderAPI::HSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	ResManager::get()->HSSetBuffer(buffers, slot);
}

void RenderAPI::DSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	ResManager::get()->DSSetBuffer(buffers, slot);
}

void RenderAPI::GSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	ResManager::get()->GSSetBuffer(buffers, slot);
}

void RenderAPI::PSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	ResManager::get()->PSSetBuffer(buffers, slot);
}

void RenderAPI::CSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot)
{
	ResManager::get()->CSSetBuffer(buffers, slot);
}

void RenderAPI::VSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num)
{
	Renderer::context->VSSetSamplers(slot, num, samplers);
}

void RenderAPI::HSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num)
{
	Renderer::context->HSSetSamplers(slot, num, samplers);
}

void RenderAPI::DSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num)
{
	Renderer::context->DSSetSamplers(slot, num, samplers);
}

void RenderAPI::GSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num)
{
	Renderer::context->GSSetSamplers(slot, num, samplers);
}

void RenderAPI::PSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num)
{
	Renderer::context->PSSetSamplers(slot, num, samplers);
}

void RenderAPI::CSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num)
{
	Renderer::context->CSSetSamplers(slot, num, samplers);
}

void RenderAPI::IASetInputLayout(ID3D11InputLayout* const layout)
{
	Renderer::context->IASetInputLayout(layout);
}

void RenderAPI::IASetVertexBuffer(const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets)
{
	ResManager::get()->IASetVertexBuffer(buffers, strides, offsets);
}
