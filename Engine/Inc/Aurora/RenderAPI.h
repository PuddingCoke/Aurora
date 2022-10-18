#pragma once

#ifndef _RENDERAPI_H_
#define _RENDERAPI_H_

#include"Renderer.h"
#include"ResManager.h"

class RenderAPI
{
public:

	static RenderAPI* get();

	RenderAPI(const RenderAPI&) = delete;

	void operator=(const RenderAPI&) = delete;

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

	void IASetVertexBuffer(const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets);

	void SetViewport(const float& width, const float& height);

	void SetViewport(const unsigned int& width, const unsigned int& height);

	void SetViewport(const int& width, const int& height);

	void SetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology);

	void SetBlendState(ID3D11BlendState* const state);

	void SetRasterState(ID3D11RasterizerState* const state);

	void SetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef);

	void DrawQuad();

	void DrawCube();

	void Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ);

	void Draw(const UINT& vertexCount, const UINT& startVertexLocation);

	void DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation);

	void DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation);

private:

	friend class Aurora;

	static RenderAPI* instance;

	RenderAPI(const unsigned int& width, const unsigned int& height, const unsigned int& msaaLevel);

	~RenderAPI();

	D3D11_VIEWPORT vp;

	RenderTargetView* defRenderTargetView;

};

#endif // !_RENDERAPI_H_
