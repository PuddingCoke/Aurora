#pragma once

#ifndef _RENDERAPI_H_
#define _RENDERAPI_H_

#include<DirectXColors.h>

#include"ResManager.h"
#include"Shader.h"
#include"States.h"

#include"CompiledShaders/FullScreenVS.h"
#include"CompiledShaders/FullScreenPS.h"
#include"CompiledShaders/ShadowVS.h"
#include"CompiledShaders/SkyboxVS.h"
#include"CompiledShaders/RandNoiseCS.h"

class RenderAPI
{
public:

	static RenderAPI* get();

	RenderAPI(const RenderAPI&) = delete;

	void operator=(const RenderAPI&) = delete;

	void OMSetDefRTV(DepthStencilView* const dsv) const;

	void ClearDefRTV(const float* const color) const;

	void OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv) const;

	void OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs) const;

	void VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const;

	void HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const;

	void DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const;

	void GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const;

	void PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const;

	void CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot) const;

	void CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot) const;

	void VSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const;

	void HSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const;

	void DSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const;

	void GSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const;

	void PSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const;

	void CSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot) const;

	void VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const;

	void HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const;

	void DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const;

	void GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const;

	void PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const;

	void CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot) const;

	void IASetInputLayout(ID3D11InputLayout* const layout) const;

	void IASetVertexBuffer(const unsigned int& slot, const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets) const;

	void IASetIndexBuffer(Buffer* const buffer, const DXGI_FORMAT& format, const UINT& offset) const;

	void RSSetViewport(const float& width, const float& height) const;

	void RSSetViewport(const unsigned int& width, const unsigned int& height) const;

	void RSSetViewport(const int& width, const int& height) const;

	void IASetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology) const;

	void OMSetBlendState(ID3D11BlendState* const state) const;

	void RSSetState(ID3D11RasterizerState* const state) const;

	void OMSetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef) const;

	void DrawQuad() const;

	void DrawCube() const;

	void Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ) const;

	void Draw(const UINT& vertexCount, const UINT& startVertexLocation) const;

	void DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation) const;

	void DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation) const;

	void VSSetShader(ID3D11VertexShader* const shader) const;

	void HSSetShader(ID3D11HullShader* const shader) const;

	void DSSetShader(ID3D11DomainShader* const shader) const;

	void GSSetShader(ID3D11GeometryShader* const shader) const;

	void PSSetShader(ID3D11PixelShader* const shader) const;

	void CSSetShader(ID3D11ComputeShader* const shader) const;

	void ResolveSubresource(ID3D11Resource* const pDstResource, const UINT& DstSubresource, ID3D11Resource* const pSrcResource, const UINT& SrcSubresource, const DXGI_FORMAT& format) const;

	void CopySubresourceRegion(ID3D11Resource* const pDstResource, const UINT& DstSubresource, const UINT& DstX, const UINT& DstY, const UINT& DstZ, ID3D11Resource* const pSrcResource, const UINT& SrcSubresource, const D3D11_BOX* const pSrcBox) const;

	void UnbindVertexBuffer() const;

	//用于阴影绘制
	void UnbindRTV() const;

	void UnbindCSUAV() const;

	void UnbindPSUAV() const;

	void GenNoise(UnorderedAccessView* const uav, const unsigned int& textureWidth, const unsigned int& textureHeight);

	void DebugDraw(ShaderResourceView* const srv);

	static Shader* fullScreenVS;

	static Shader* fullScreenPS;

	static Shader* skyboxVS;

	static Shader* shadowVS;

	static Shader* randNoiseCS;

private:

	friend class Aurora;

	static RenderAPI* instance;

	RenderAPI(const unsigned int& width, const unsigned int& height, const unsigned int& msaaLevel, ID3D11Texture2D* const renderTexture);

	~RenderAPI();

	RenderTargetView* defRenderTargetView;

};

#endif // !_RENDERAPI_H_
