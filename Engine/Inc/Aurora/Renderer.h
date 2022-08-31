#pragma once

#include<d3d11_4.h>
#include<wrl/client.h>
#include<DirectXMath.h>

using Microsoft::WRL::ComPtr;

class Renderer
{
public:

	static ComPtr<ID3D11Device4> device;

	static ComPtr<ID3D11DeviceContext4> context;

	static D3D11_VIEWPORT vp;

	static void setDefRTV(ID3D11DepthStencilView* const view = nullptr);

	static void clearDefRTV(const float* color);

	static void setViewport(const float& vWidth, const float& vHeight);

	static void setTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology);

	static void setBlendState(ID3D11BlendState* const blendState);

	static void setSampler(const unsigned int& slot, ID3D11SamplerState* const state);

	static void draw(const UINT& vertexCount,const UINT& startVertexLocation);

	static void drawIndexed(const UINT& indexCount, const UINT& startIndexLocation, const UINT& baseVertexLocation);

	static void drawInstanced(const UINT& vertexCountPerInstance, const UINT& instanceCount, const UINT& startVertexLocation, const UINT& startInstanceLocation);

private:

	friend class Aurora;

	static ComPtr<ID3D11RenderTargetView> defaultTargetView;

	static ID3D11Texture2D* backBuffer;

};