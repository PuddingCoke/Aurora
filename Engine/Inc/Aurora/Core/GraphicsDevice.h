#pragma once

#ifndef _GRAPHICSDEVICE_H_
#define _GRAPHICSDEVICE_H_

#include<d3d11_4.h>
#include<DirectXColors.h>
#include<wrl/client.h>
#include<comdef.h>
#include<iostream>

#include"ResourceFormat.h"

using Microsoft::WRL::ComPtr;

class GraphicsDevice
{
public:

	GraphicsDevice(const GraphicsDevice&) = delete;

	void operator=(const GraphicsDevice&) = delete;

	static GraphicsDevice* get();

	void createBuffer(const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Buffer** address) const;

	void createTexture2D(const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture2D** address) const;

	void createTexture3D(const D3D11_TEXTURE3D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture3D** address) const;

	void createDepthStencilView(ID3D11Resource* resource, const D3D11_DEPTH_STENCIL_VIEW_DESC* desc, ID3D11DepthStencilView** address) const;

	void createShaderResourceView(ID3D11Resource* resource, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc, ID3D11ShaderResourceView** address) const;

	void createRenderTargetView(ID3D11Resource* resource, const D3D11_RENDER_TARGET_VIEW_DESC* desc, ID3D11RenderTargetView** address) const;

	void createUnorderedAccessView(ID3D11Resource* resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC* desc, ID3D11UnorderedAccessView** address) const;

	void createVertexShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11VertexShader** address) const;

	void createHullShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11HullShader** address) const;

	void createDomainShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11DomainShader** address) const;

	void createGeometryShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11GeometryShader** address) const;

	void createPixelShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11PixelShader** address) const;

	void createComputeShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11ComputeShader** address) const;

	void createSamplerState(const D3D11_SAMPLER_DESC* desc, ID3D11SamplerState** address) const;

	void createInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, UINT arraySize, const void* byteCode, SIZE_T byteCodeLength, ID3D11InputLayout** address) const;

	void createDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* desc, ID3D11DepthStencilState** address) const;

	void createBlendState(const D3D11_BLEND_DESC* desc, ID3D11BlendState** address) const;

	void createRasterizerState(const D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** address) const;

	void createRasterizerState1(const D3D11_RASTERIZER_DESC1* desc, ID3D11RasterizerState1** address) const;

	void createRasterizerState2(const D3D11_RASTERIZER_DESC2* desc, ID3D11RasterizerState2** address) const;

	static ID3D11Device5* getDevice();

private:

	friend class Aurora;

	GraphicsDevice();

	static GraphicsDevice* instance;

	ComPtr<ID3D11Device5> device;

};

#define CHECKERROR(x) \
HRESULT hr = x;\
if(FAILED(hr))\
{\
std::cout<<__FILE__<<" Line:"<<__LINE__<<"\n";\
std::cout<<"Function name "<<__FUNCTION__ <<"\n";\
std::cout<<"Failed with 0x"<<std::hex<<hr<<std::dec<<"\n";\
_com_error err(hr);\
std::wcout<<"Failed reason "<<err.ErrorMessage()<<"\n";\
__debugbreak();\
}\

#endif // !_GRAPHICSDEVICE_H_
