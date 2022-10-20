#include<Aurora/RenderAPI.h>

RenderAPI* RenderAPI::instance = nullptr;

RenderAPI::RenderAPI(const unsigned int& width, const unsigned int& height, const unsigned int& msaaLevel)
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

RenderAPI* RenderAPI::get()
{
	return instance;
}

RenderAPI::~RenderAPI()
{
	delete defRenderTargetView;
}

void RenderAPI::CreateSamplerState(const D3D11_SAMPLER_DESC& desc, ID3D11SamplerState** const state) const
{
	Renderer::device->CreateSamplerState(&desc, state);
}

void RenderAPI::CreateBlendState(const D3D11_BLEND_DESC& desc, ID3D11BlendState** const state) const
{
	Renderer::device->CreateBlendState(&desc, state);
}

void RenderAPI::CreateRasterizerState(const D3D11_RASTERIZER_DESC& desc, ID3D11RasterizerState** const state) const
{
	Renderer::device->CreateRasterizerState(&desc, state);
}

void RenderAPI::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc, ID3D11DepthStencilState** const state) const
{
	Renderer::device->CreateDepthStencilState(&desc, state);
}

void RenderAPI::OMSetDefRTV(ID3D11DepthStencilView* const dsv) const
{
	ResManager::get()->OMSetRTV({ defRenderTargetView }, dsv);
}

void RenderAPI::ClearDefRTV(const float* const color) const
{
	defRenderTargetView->clearRTV(color);
}

void RenderAPI::OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, ID3D11DepthStencilView* const dsv) const
{
	ResManager::get()->OMSetRTV(rtvs, dsv);
}

void RenderAPI::VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const
{
	ResManager::get()->VSSetSRV(srvs, slot);
}

void RenderAPI::HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const
{
	ResManager::get()->HSSetSRV(srvs, slot);
}

void RenderAPI::DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const
{
	ResManager::get()->DSSetSRV(srvs, slot);
}

void RenderAPI::GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const
{
	ResManager::get()->GSSetSRV(srvs, slot);
}

void RenderAPI::PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const
{
	ResManager::get()->PSSetSRV(srvs, slot);
}

void RenderAPI::CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const
{
	ResManager::get()->CSSetSRV(srvs, slot);
}

void RenderAPI::CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot) const
{
	ResManager::get()->CSSetUAV(uavs, slot);
}

void RenderAPI::VSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot) const
{
	ResManager::get()->VSSetBuffer(buffers, slot);
}

void RenderAPI::HSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot) const
{
	ResManager::get()->HSSetBuffer(buffers, slot);
}

void RenderAPI::DSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot) const
{
	ResManager::get()->DSSetBuffer(buffers, slot);
}

void RenderAPI::GSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot) const
{
	ResManager::get()->GSSetBuffer(buffers, slot);
}

void RenderAPI::PSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot) const
{
	ResManager::get()->PSSetBuffer(buffers, slot);
}

void RenderAPI::CSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot) const
{
	ResManager::get()->CSSetBuffer(buffers, slot);
}

void RenderAPI::VSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num) const
{
	Renderer::context->VSSetSamplers(slot, num, samplers);
}

void RenderAPI::HSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num) const
{
	Renderer::context->HSSetSamplers(slot, num, samplers);
}

void RenderAPI::DSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num) const
{
	Renderer::context->DSSetSamplers(slot, num, samplers);
}

void RenderAPI::GSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num) const
{
	Renderer::context->GSSetSamplers(slot, num, samplers);
}

void RenderAPI::PSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num) const
{
	Renderer::context->PSSetSamplers(slot, num, samplers);
}

void RenderAPI::CSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num) const
{
	Renderer::context->CSSetSamplers(slot, num, samplers);
}

void RenderAPI::IASetInputLayout(ID3D11InputLayout* const layout) const
{
	Renderer::context->IASetInputLayout(layout);
}

void RenderAPI::IASetVertexBuffer(const unsigned int& slot, const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets) const
{
	ResManager::get()->IASetVertexBuffer(slot, buffers, strides, offsets);
}

void RenderAPI::IASetIndexBuffer(Buffer* const buffer, const DXGI_FORMAT& format, const UINT& offset) const
{
	Renderer::context->IASetIndexBuffer(buffer->getBuffer(), format, offset);
}

void RenderAPI::RSSetViewport(const float& width, const float& height) const
{
	Renderer::instance->vp.Width = width;
	Renderer::instance->vp.Height = height;
	Renderer::context->RSSetViewports(1, &Renderer::instance->vp);
}

void RenderAPI::RSSetViewport(const unsigned int& width, const unsigned int& height) const
{
	RSSetViewport((float)width, (float)height);
}

void RenderAPI::RSSetViewport(const int& width, const int& height) const
{
	RSSetViewport((float)width, (float)height);
}

void RenderAPI::IASetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology) const
{
	Renderer::context->IASetPrimitiveTopology(topology);
}

void RenderAPI::OMSetBlendState(ID3D11BlendState* const state) const
{
	Renderer::context->OMSetBlendState(state, nullptr, 0xFFFFFFFF);
}

void RenderAPI::RSSetState(ID3D11RasterizerState* const state) const
{
	Renderer::context->RSSetState(state);
}

void RenderAPI::OMSetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef) const
{
	Renderer::context->OMSetDepthStencilState(state, stencilRef);
}

void RenderAPI::DrawQuad() const
{
	Renderer::context->Draw(3, 0);
}

void RenderAPI::DrawCube() const
{
	Renderer::context->Draw(36, 0);
}

void RenderAPI::Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ) const
{
	Renderer::context->Dispatch(threadGroupCountX, threadGroupCountY, threadGroupCountZ);
}

void RenderAPI::Draw(const UINT& vertexCount, const UINT& startVertexLocation) const
{
	Renderer::context->Draw(vertexCount, startVertexLocation);
}

void RenderAPI::DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation) const
{
	Renderer::context->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void RenderAPI::DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation) const
{
	Renderer::context->DrawInstanced(vertexCountPerInstance, instanceCount, startVertexLocation, startInstanceLocation);
}

void RenderAPI::VSSetShader(ID3D11VertexShader* const shader) const
{
	Renderer::context->VSSetShader(shader, nullptr, 0);
}

void RenderAPI::HSSetShader(ID3D11HullShader* const shader) const
{
	Renderer::context->HSSetShader(shader, nullptr, 0);
}

void RenderAPI::DSSetShader(ID3D11DomainShader* const shader) const
{
	Renderer::context->DSSetShader(shader, nullptr, 0);
}

void RenderAPI::GSSetShader(ID3D11GeometryShader* const shader) const
{
	Renderer::context->GSSetShader(shader, nullptr, 0);
}

void RenderAPI::PSSetShader(ID3D11PixelShader* const shader) const
{
	Renderer::context->PSSetShader(shader, nullptr, 0);
}

void RenderAPI::CSSetShader(ID3D11ComputeShader* const shader) const
{
	Renderer::context->CSSetShader(shader, nullptr, 0);
}

void RenderAPI::ResolveSubresource(ID3D11Resource* const pDstResource, const UINT& DstSubresource, ID3D11Resource* const pSrcResource, const UINT& SrcSubresource, const DXGI_FORMAT& format) const
{
	Renderer::context->ResolveSubresource(pDstResource, DstSubresource, pSrcResource, SrcSubresource, format);
}

void RenderAPI::CopySubresourceRegion(ID3D11Resource* const pDstResource, const UINT& DstSubresource, const UINT& DstX, const UINT& DstY, const UINT& DstZ, ID3D11Resource* const pSrcResource, const UINT& SrcSubresource, const D3D11_BOX* const pSrcBox) const
{
	Renderer::context->CopySubresourceRegion(pDstResource, DstSubresource, DstX, DstY, DstZ, pSrcResource, SrcSubresource, pSrcBox);
}

void RenderAPI::UnbindRTV() const
{
	RenderTargetView::unbindRTV();
}

void RenderAPI::UnbindUAV() const
{
	UnorderedAccessView::unbindUAV();
}
