#include<Aurora/RenderAPI.h>

RenderAPI* RenderAPI::instance = nullptr;

Shader* RenderAPI::fullScreenVS;
Shader* RenderAPI::fullScreenPS;
Shader* RenderAPI::skyboxVS;
Shader* RenderAPI::shadowVS;
Shader* RenderAPI::randNoiseCS;

RenderAPI::RenderAPI(const unsigned int& width, const unsigned int& height, const unsigned int& msaaLevel, ID3D11Texture2D* const renderTexture)
{
	if (msaaLevel == 1)
	{
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
		rtvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = 0;

		defRenderTargetView = new RenderTargetView(renderTexture, rtvDesc);
	}
	else
	{
		D3D11_RENDER_TARGET_VIEW_DESC msaaViewDesc = {};
		msaaViewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		msaaViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

		defRenderTargetView = new RenderTargetView(Renderer::instance->msaaTexture.Get(), msaaViewDesc);
	}

	std::cout << "fullScreenVS ";
	fullScreenVS = new Shader(g_FullScreenVSBytes, sizeof(g_FullScreenVSBytes), ShaderType::Vertex);
	std::cout << "fullScreenPS ";
	fullScreenPS = new Shader(g_FullScreenPSBytes, sizeof(g_FullScreenPSBytes), ShaderType::Pixel);
	std::cout << "skyboxVS ";
	skyboxVS = new Shader(g_SkyboxVSBytes, sizeof(g_SkyboxVSBytes), ShaderType::Vertex);
	std::cout << "shadowVS ";
	shadowVS = new Shader(g_ShadowVSBytes, sizeof(g_ShadowVSBytes), ShaderType::Vertex);
	std::cout << "randNoiseCS ";
	randNoiseCS = new Shader(g_RandNoiseCSBytes, sizeof(g_RandNoiseCSBytes), ShaderType::Compute);
}

RenderAPI* RenderAPI::get()
{
	return instance;
}

RenderAPI::~RenderAPI()
{
	delete defRenderTargetView;
	delete fullScreenVS;
	delete fullScreenPS;
	delete skyboxVS;
	delete shadowVS;
	delete randNoiseCS;
}

void RenderAPI::OMSetDefRTV(DepthStencilView* const dsv) const
{
	ResManager::get()->OMSetRTV({ defRenderTargetView }, dsv);
}

void RenderAPI::ClearDefRTV(const float* const color) const
{
	defRenderTargetView->clearRTV(color);
}

void RenderAPI::OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv) const
{
	ResManager::get()->OMSetRTV(rtvs, dsv);
}

void RenderAPI::OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs) const
{
	ResManager::get()->OMSetUAV(uavs);
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

void RenderAPI::VSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const
{
	ResManager::get()->VSSetConstantBuffer(constantBuffers, slot);
}

void RenderAPI::HSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const
{
	ResManager::get()->HSSetConstantBuffer(constantBuffers, slot);
}

void RenderAPI::DSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const
{
	ResManager::get()->DSSetConstantBuffer(constantBuffers, slot);
}

void RenderAPI::GSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const
{
	ResManager::get()->GSSetConstantBuffer(constantBuffers, slot);
}

void RenderAPI::PSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const
{
	ResManager::get()->PSSetConstantBuffer(constantBuffers, slot);
}

void RenderAPI::CSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const
{
	ResManager::get()->CSSetConstantBuffer(constantBuffers, slot);
}

void RenderAPI::VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const
{
	ResManager::get()->VSSetSampler(samplers, slot);
}

void RenderAPI::HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const
{
	ResManager::get()->HSSetSampler(samplers, slot);
}

void RenderAPI::DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const
{
	ResManager::get()->DSSetSampler(samplers, slot);
}

void RenderAPI::GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const
{
	ResManager::get()->GSSetSampler(samplers, slot);
}

void RenderAPI::PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const
{
	ResManager::get()->PSSetSampler(samplers, slot);
}

void RenderAPI::CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const
{
	ResManager::get()->CSSetSampler(samplers, slot);
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

void RenderAPI::UnbindVertexBuffer() const
{
	Buffer::unbindVertexBuffer();
}

void RenderAPI::UnbindRTV() const
{
	RenderTargetView::unbindRTV();
}

void RenderAPI::UnbindCSUAV() const
{
	UnorderedAccessView::unbindCUAV();
}

void RenderAPI::UnbindPSUAV() const
{
	UnorderedAccessView::unbindPUAV();
}

void RenderAPI::GenNoise(UnorderedAccessView* const uav, const unsigned int& textureWidth, const unsigned int& textureHeight)
{
	randNoiseCS->use();
	CSSetUAV({ uav }, 0);
	RenderAPI::get()->Dispatch(textureWidth / 32, textureHeight / 18, 1);
}

void RenderAPI::DebugDraw(ShaderResourceView* const srv)
{
	IASetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	OMSetBlendState(nullptr);
	ClearDefRTV(DirectX::Colors::Black);
	OMSetDefRTV(nullptr);
	PSSetSRV({ srv }, 0);
	PSSetSampler({ States::pointClampSampler }, 0);

	fullScreenVS->use();
	fullScreenPS->use();
	DSSetShader(nullptr);
	HSSetShader(nullptr);
	GSSetShader(nullptr);

	RenderAPI::get()->DrawQuad();
}
