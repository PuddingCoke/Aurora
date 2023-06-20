#include<Aurora/Core/GraphicsDevice.h>

GraphicsDevice* GraphicsDevice::instance = nullptr;

GraphicsDevice* GraphicsDevice::get()
{
	return instance;
}

void GraphicsDevice::createBuffer(const D3D11_BUFFER_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Buffer** address) const
{
	CHECKERROR(device->CreateBuffer(desc, initialData, address));
}

void GraphicsDevice::createTexture2D(const D3D11_TEXTURE2D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture2D** address) const
{
	CHECKERROR(device->CreateTexture2D(desc, initialData, address));
}

void GraphicsDevice::createTexture3D(const D3D11_TEXTURE3D_DESC* desc, const D3D11_SUBRESOURCE_DATA* initialData, ID3D11Texture3D** address) const
{
	CHECKERROR(device->CreateTexture3D(desc, initialData, address));
}

void GraphicsDevice::createDepthStencilView(ID3D11Resource* resource, const D3D11_DEPTH_STENCIL_VIEW_DESC* desc, ID3D11DepthStencilView** address) const
{
	CHECKERROR(device->CreateDepthStencilView(resource, desc, address));
}

void GraphicsDevice::createShaderResourceView(ID3D11Resource* resource, const D3D11_SHADER_RESOURCE_VIEW_DESC* desc, ID3D11ShaderResourceView** address) const
{
	CHECKERROR(device->CreateShaderResourceView(resource, desc, address));
}

void GraphicsDevice::createRenderTargetView(ID3D11Resource* resource, const D3D11_RENDER_TARGET_VIEW_DESC* desc, ID3D11RenderTargetView** address) const
{
	CHECKERROR(device->CreateRenderTargetView(resource, desc, address));
}

void GraphicsDevice::createUnorderedAccessView(ID3D11Resource* resource, const D3D11_UNORDERED_ACCESS_VIEW_DESC* desc, ID3D11UnorderedAccessView** address) const
{
	CHECKERROR(device->CreateUnorderedAccessView(resource, desc, address));
}

void GraphicsDevice::createVertexShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11VertexShader** address) const
{
	CHECKERROR(device->CreateVertexShader(byteCode, byteCodeLength, nullptr, address));
}

void GraphicsDevice::createHullShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11HullShader** address) const
{
	CHECKERROR(device->CreateHullShader(byteCode, byteCodeLength, nullptr, address));
}

void GraphicsDevice::createDomainShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11DomainShader** address) const
{
	CHECKERROR(device->CreateDomainShader(byteCode, byteCodeLength, nullptr, address));
}

void GraphicsDevice::createGeometryShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11GeometryShader** address) const
{
	CHECKERROR(device->CreateGeometryShader(byteCode, byteCodeLength, nullptr, address));
}

void GraphicsDevice::createPixelShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11PixelShader** address) const
{
	CHECKERROR(device->CreatePixelShader(byteCode, byteCodeLength, nullptr, address));
}

void GraphicsDevice::createComputeShader(const void* byteCode, SIZE_T byteCodeLength, ID3D11ComputeShader** address) const
{
	CHECKERROR(device->CreateComputeShader(byteCode, byteCodeLength, nullptr, address));
}

void GraphicsDevice::createSamplerState(const D3D11_SAMPLER_DESC* desc, ID3D11SamplerState** address) const
{
	CHECKERROR(device->CreateSamplerState(desc, address));
}

void GraphicsDevice::createInputLayout(const D3D11_INPUT_ELEMENT_DESC* desc, UINT arraySize, const void* byteCode, SIZE_T byteCodeLength, ID3D11InputLayout** address) const
{
	CHECKERROR(device->CreateInputLayout(desc, arraySize, byteCode, byteCodeLength, address));
}

void GraphicsDevice::createDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* desc, ID3D11DepthStencilState** address) const
{
	CHECKERROR(device->CreateDepthStencilState(desc, address));
}

void GraphicsDevice::createBlendState(const D3D11_BLEND_DESC* desc, ID3D11BlendState** address) const
{
	CHECKERROR(device->CreateBlendState(desc, address));
}

void GraphicsDevice::createRasterizerState(const D3D11_RASTERIZER_DESC* desc, ID3D11RasterizerState** address) const
{
	CHECKERROR(device->CreateRasterizerState(desc, address));
}

void GraphicsDevice::createRasterizerState1(const D3D11_RASTERIZER_DESC1* desc, ID3D11RasterizerState1** address) const
{
	CHECKERROR(device->CreateRasterizerState1(desc, address));
}

void GraphicsDevice::createRasterizerState2(const D3D11_RASTERIZER_DESC2* desc, ID3D11RasterizerState2** address) const
{
	CHECKERROR(device->CreateRasterizerState2(desc, address));
}

ID3D11Device5* GraphicsDevice::getDevice()
{
	return instance->device.Get();
}

GraphicsDevice::GraphicsDevice()
{
	instance = this;
}
