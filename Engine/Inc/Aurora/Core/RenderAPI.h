#pragma once

#ifndef _RENDERAPI_H_
#define _RENDERAPI_H_

#include<DirectXColors.h>

#include"Shader.h"
#include"States.h"
#include"Graphics.h"

#include"DX/View/RenderTargetView.h"
#include"DX/View/UnorderedAccessView.h"
#include"DX/View/ShaderResourceView.h"
#include"DX/View/DepthStencilView.h"

#include"DX/Resource/Buffer.h"

#include"../CompiledShaders/FullScreenVS.h"
#include"../CompiledShaders/FullScreenPS.h"
#include"../CompiledShaders/ShadowVS.h"
#include"../CompiledShaders/SkyboxVS.h"
#include"../CompiledShaders/RandNoiseCS.h"

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

	void VSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void HSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void DSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void GSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void PSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void CSSetSRV(const std::initializer_list<ShaderResourceView*>& srvs, const unsigned int& slot);

	void CSSetUAV(const std::initializer_list<UnorderedAccessView*>& uavs, const unsigned int& slot);

	void VSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void HSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void DSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void GSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void PSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void CSSetConstantBuffer(const std::initializer_list<Buffer*>& constantBuffers, const unsigned int& slot);

	void VSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void HSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void DSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void GSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void PSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void CSSetSampler(const std::initializer_list<ID3D11SamplerState*>& samplers, const unsigned int& slot);

	void IASetVertexBuffer(const unsigned int& slot, const std::initializer_list<Buffer*>& buffers, const std::initializer_list<UINT>& strides, const std::initializer_list<UINT>& offsets);

	void IASetInputLayout(ID3D11InputLayout* const layout) const;

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

	RenderAPI(const unsigned int& msaaLevel, ID3D11Texture2D* const renderTexture);

	~RenderAPI();

	class CustomedRTV :public RenderTargetView
	{
	public:

		CustomedRTV(ID3D11Resource* const resource, const D3D11_RENDER_TARGET_VIEW_DESC& rtvDesc)
		{
			createRTV(resource, rtvDesc);
		}

		void bindRTV() override
		{

		}

	}*defRenderTargetView;

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
