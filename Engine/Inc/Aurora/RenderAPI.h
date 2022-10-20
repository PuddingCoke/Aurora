#pragma once

#ifndef _RENDERAPI_H_
#define _RENDERAPI_H_

#include"Renderer.h"
#include"ResManager.h"
#include"Shader.h"

class RenderAPI
{
public:

	static RenderAPI* get();

	RenderAPI(const RenderAPI&) = delete;

	void operator=(const RenderAPI&) = delete;

	void CreateSamplerState(const D3D11_SAMPLER_DESC& desc, ID3D11SamplerState** const state);

	void CreateBlendState(const D3D11_BLEND_DESC& desc, ID3D11BlendState** const state);

	void CreateRasterizerState(const D3D11_RASTERIZER_DESC& desc, ID3D11RasterizerState** const state);

	void CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC& desc, ID3D11DepthStencilState** const state);

	void OMSetDefRTV(ID3D11DepthStencilView* const dsv);

	void ClearDefRTV(const float* const color);

	void OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, ID3D11DepthStencilView* const dsv);

	void VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot);

	void VSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void HSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void DSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void GSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void PSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void CSSetBuffer(const std::initializer_list<Buffer*>& buffers, const unsigned int& slot);

	void VSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num);

	void HSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num);

	void DSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num);

	void GSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num);

	void PSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num);

	void CSSetSampler(ID3D11SamplerState** const samplers, const unsigned int& slot, const unsigned int& num);

	void IASetInputLayout(ID3D11InputLayout* const layout);

	void IASetVertexBuffer(const unsigned int& slot, const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets);

	void IASetIndexBuffer(Buffer* const buffer, const DXGI_FORMAT& format, const UINT& offset);

	void RSSetViewport(const float& width, const float& height);

	void RSSetViewport(const unsigned int& width, const unsigned int& height);

	void RSSetViewport(const int& width, const int& height);

	void IASetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology);

	void OMSetBlendState(ID3D11BlendState* const state);

	void RSSetState(ID3D11RasterizerState* const state);

	void OMSetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef);

	void DrawQuad();

	void DrawCube();

	void Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ);

	void Draw(const UINT& vertexCount, const UINT& startVertexLocation);

	void DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation);

	void DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation);

	void VSSetShader(ID3D11VertexShader* const shader);

	void HSSetShader(ID3D11HullShader* const shader);

	void DSSetShader(ID3D11DomainShader* const shader);

	void GSSetShader(ID3D11GeometryShader* const shader);

	void PSSetShader(ID3D11PixelShader* const shader);

	void CSSetShader(ID3D11ComputeShader* const shader);

	void ResolveSubresource(ID3D11Resource* const pDstResource, const UINT& DstSubresource, ID3D11Resource* const pSrcResource, const UINT& SrcSubresource, const DXGI_FORMAT& format);

	void CopySubresourceRegion(ID3D11Resource* const pDstResource, const UINT& DstSubresource, const UINT& DstX, const UINT& DstY, const UINT& DstZ, ID3D11Resource* const pSrcResource, const UINT& SrcSubresource, const D3D11_BOX* const pSrcBox);

	//用于阴影绘制
	void UnbindRTV();

private:

	friend class Aurora;

	static RenderAPI* instance;

	RenderAPI(const unsigned int& width, const unsigned int& height, const unsigned int& msaaLevel);

	~RenderAPI();

	D3D11_VIEWPORT vp;

	RenderTargetView* defRenderTargetView;

};

#endif // !_RENDERAPI_H_
