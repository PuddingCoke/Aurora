#pragma once

#ifndef _RENDERAPI_H_
#define _RENDERAPI_H_

#include<DirectXColors.h>

#include<Aurora/Core/Shader.h>

#include"States.h"
#include"Graphics.h"

#include<Aurora/Core/DX/View/RenderTargetView.h>
#include<Aurora/Core/DX/View/UnorderedAccessView.h>
#include<Aurora/Core/DX/View/ShaderResourceView.h>
#include<Aurora/Core/DX/View/DepthStencilView.h>
#include<Aurora/Core/DX/View/Special/RenderOnlyRTV.h>

#include<Aurora/Core/DX/Resource/Buffer.h>

#include<Aurora/Resource/ConstantBuffer.h>
#include<Aurora/Resource/VertexBuffer.h>

#include<Aurora/CompiledShaders/FullScreenVS.h>
#include<Aurora/CompiledShaders/FullScreenPS.h>
#include<Aurora/CompiledShaders/ShadowVS.h>
#include<Aurora/CompiledShaders/SkyboxVS.h>
#include<Aurora/CompiledShaders/RandNoiseCS.h>

class RenderAPI
{
public:

	static RenderAPI* get();

	RenderAPI(const RenderAPI&) = delete;

	void operator=(const RenderAPI&) = delete;

	void ClearDefRTV(const float* const color) const;

	void OMSetDefRTV(DepthStencilView* const dsv);

	void OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv);

	void OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs);

	void CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const UINT& slot);

	void VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot);

	void HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot);

	void DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot);

	void GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot);

	void PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot);

	void CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const UINT& slot);

	void VSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot);

	void HSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot);

	void DSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot);

	void GSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot);

	void PSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot);

	void CSSetConstantBuffer(const std::initializer_list<ConstantBuffer*>& constantBuffers, const UINT& slot);

	void VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot);

	void HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot);

	void DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot);

	void GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot);

	void PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot);

	void CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const UINT& slot);

	void IASetVertexBuffer(const UINT& slot, const std::initializer_list<VertexBuffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets);

	void IASetInputLayout(ID3D11InputLayout* const layout) const;

	void IASetIndexBuffer(Buffer* const buffer, const DXGI_FORMAT& format, const UINT& offset) const;

	void RSSetViewport(const float& width, const float& height) const;

	void RSSetViewport(const UINT& width, const UINT& height) const;

	void RSSetViewport(const int& width, const int& height) const;

	void IASetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology) const;

	void OMSetBlendState(ID3D11BlendState* const state) const;

	void RSSetState(ID3D11RasterizerState* const state) const;

	void OMSetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef) const;

	void ResolveSubresource(const Resource* const dstResource, const UINT& dstSubresource, const Resource* const srcResource, const UINT& srcSubresource, const DXGI_FORMAT& format);

	void DrawQuad() const;

	void DrawCube() const;

	void Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ) const;

	void Draw(const UINT& vertexCount, const UINT& startVertexLocation) const;

	void DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation) const;

	void DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation) const;

	void VSUnbindShader() const;

	void HSUnbindShader() const;

	void DSUnbindShader() const;

	void GSUnbindShader() const;

	void PSUnbindShader() const;

	void CSUnbindShader() const;

	void UnbindRTV() const;

	void UnbindCSUAV() const;

	void UnbindPSUAV() const;

	void GenNoise(UnorderedAccessView* const uav, const UINT& textureWidth, const UINT& textureHeight);

	void DebugDraw(ShaderResourceView* const srv);

	static Shader* fullScreenVS;

	static Shader* fullScreenPS;

	static Shader* skyboxVS;

	static Shader* shadowVS;

	static Shader* randNoiseCS;

private:

	friend class Aurora;

	static RenderAPI* instance;

	RenderAPI(const UINT& msaaLevel, ID3D11Texture2D* const renderTexture);

	~RenderAPI();

	RenderOnlyRTV* defRenderTargetView;

	ID3D11RenderTargetView* tempRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ID3D11UnorderedAccessView* tempUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	ID3D11ShaderResourceView* tempSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ID3D11Buffer* tempBuffer[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	UINT tempStartConstants[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	UINT tempNumConstants[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	ID3D11SamplerState* tempSampler[D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT];

	UINT tempStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT], tempOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

};

#endif // !_RENDERAPI_H_
