#pragma once

#ifndef _GRAPHICSCONTEXT_H_
#define _GRAPHICSCONTEXT_H_

#include<DirectXColors.h>

#include<Aurora/Core/Shader.h>

#include<Aurora/Core/DX/View/Special/RenderOnlyRTV.h>
#include<Aurora/Core/DX/View/DepthStencilView.h>
#include<Aurora/Core/DX/Resource/Buffer.h>

#include<Aurora/Resource/ConstantBuffer.h>

#include<Aurora/Core/GraphicsStates.h>
#include<Aurora/Core/GraphicsDevice.h>

class GraphicsContext
{
public:

	GraphicsContext();

	~GraphicsContext();

	void ClearDefRTV(const float* const color);

	void OMSetDefRTV(DepthStencilView* const dsv);

	void OMSetRTV(const std::initializer_list<RenderTargetView*>& rtvs, DepthStencilView* const dsv);

	void OMSetUAV(const std::initializer_list<UnorderedAccessView*> uavs);

	void BindShader(Shader* const shader);

	void ClearDSV(DepthStencilView* dsv, const UINT& clearFlag, const float& depth = 1.0f, const UINT8& stencil = 0);

	void ClearRTV(RenderTargetView* rtv, const float* const color);

	void ClearUAV(UnorderedAccessView* uav, const float* const color);

	void ClearUAV(UnorderedAccessView* uav, const unsigned int* const value);

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

	void IASetInputLayout(ID3D11InputLayout* const layout);

	void IASetIndexBuffer(Buffer* const buffer, const DXGI_FORMAT& format, const UINT& offset);

	void RSSetViewport(const float& width, const float& height);

	void RSSetViewport(const UINT& width, const UINT& height);

	void RSSetViewport(const int& width, const int& height);

	void IASetTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology);

	void OMSetBlendState(ID3D11BlendState* const state);

	void RSSetState(ID3D11RasterizerState* const state);

	void OMSetDepthStencilState(ID3D11DepthStencilState* const state, const UINT& stencilRef);

	void ResolveSubresource(const Resource* const dstResource, const UINT& dstSubresource, const Resource* const srcResource, const UINT& srcSubresource, const DXGI_FORMAT& format);

	void DrawQuad();

	void DrawCube();

	void Dispatch(const UINT& threadGroupCountX, const UINT& threadGroupCountY, const UINT& threadGroupCountZ);

	void Draw(const UINT& vertexCount, const UINT& startVertexLocation);

	void DrawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation);

	void DrawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation);

	void Map(Resource* const res, const UINT& subresource, const D3D11_MAP& mapType, const UINT& mapFlag, D3D11_MAPPED_SUBRESOURCE* const mappedSubresource);

	void Unmap(Resource* const res, const UINT& subresource);

	void UpdateSubresource(Resource* const res, const UINT& dstSubresource, const D3D11_BOX* const pDstBox, const void* const data, const UINT& rowPitch, const UINT& depthPitch);

	void FinishCommandList(ID3D11CommandList** commandList);

	void GenerateMips(ShaderResourceView* const srv);

	void CopySubresourceRegion(Resource* const pDstResource, const UINT& DstSubresource, const UINT& DstX, const UINT& DstY, const UINT& DstZ, Resource* const pSrcResource, const UINT& SrcSubresource, const D3D11_BOX* const pSrcBox);

	void VSUnbindShader();

	void HSUnbindShader();

	void DSUnbindShader();

	void GSUnbindShader();

	void PSUnbindShader();

	void CSUnbindShader();

	void ClearState();

	void UnbindRTV();

	void UnbindCSUAV();

	void UnbindPSUAV();

	void GenNoise(UnorderedAccessView* const uav, const UINT& textureWidth, const UINT& textureHeight);

	void ResetStates();

	ID3D11DeviceContext3* getContext();

	GraphicsStates* getStates();

protected:

	static RenderOnlyRTV* defRenderTargetView;

private:

	friend class Aurora;

	ComPtr<ID3D11DeviceContext3> context;

	D3D11_VIEWPORT vp;

	GraphicsStates states;

	ID3D11RenderTargetView* tempRTV[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

	ID3D11UnorderedAccessView* tempUAV[D3D11_PS_CS_UAV_REGISTER_COUNT];

	ID3D11ShaderResourceView* tempSRV[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

	ID3D11Buffer* tempBuffer[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	UINT tempStartConstants[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	UINT tempNumConstants[D3D11_REQ_CONSTANT_BUFFER_ELEMENT_COUNT];

	ID3D11SamplerState* tempSampler[D3D11_COMMONSHADER_SAMPLER_REGISTER_COUNT];

	UINT tempStrides[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT], tempOffsets[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

};

#endif // !_GRAPHICSCONTEXT_H_
