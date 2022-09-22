#include<Aurora/A3D/ShadowMap.h>

Shader* ShadowMap::shadowVShader;

ShadowMap* ShadowMap::create(const unsigned int& width, const unsigned int& height)
{
	return new ShadowMap(width, height);
}

void ShadowMap::setSRV(const UINT& slot) const
{
	Renderer::context->PSSetShaderResources(slot, 1, shadowSRV.GetAddressOf());
}

void ShadowMap::clear(const float& depth) const
{
	Renderer::context->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH, depth, 0);
}

ID3D11DepthStencilView* ShadowMap::get() const
{
	return depthStencilView.Get();
}

ID3D11DepthStencilView* ShadowMap::getROView() const
{
	return depthStencilViewRO.Get();
}

ID3D11ShaderResourceView* ShadowMap::getSRV() const
{
	return shadowSRV.Get();
}

void ShadowMap::ini()
{
	{
		const std::string source = R"(
struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    matrix normalMatrix;
}

cbuffer Light : register(b2)
{
    matrix lightVewProj;
};

float4 main(VertexInput input) : SV_POSITION
{
    return mul(float4(input.pos, 1.0), lightVewProj);
}
)";

		std::cout << "shadowVShader ";

		shadowVShader = Shader::fromStr(source, ShaderType::Vertex);

	}
}

void ShadowMap::release()
{
	delete shadowVShader;
}

ShadowMap::ShadowMap(const unsigned int& width, const unsigned int& height)
{
	D3D11_TEXTURE2D_DESC tDesc = {};
	tDesc.Width = width;
	tDesc.Height = height;
	tDesc.MipLevels = 1;
	tDesc.ArraySize = 1;
	tDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	Renderer::device->CreateTexture2D(&tDesc, nullptr, shadowTexture.ReleaseAndGetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	Renderer::device->CreateDepthStencilView(shadowTexture.Get(), &dsvDesc, depthStencilView.ReleaseAndGetAddressOf());

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvRODesc = {};
	dsvRODesc.Flags = D3D11_DSV_READ_ONLY_DEPTH;
	dsvRODesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvRODesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvRODesc.Texture2D.MipSlice = 0;

	Renderer::device->CreateDepthStencilView(shadowTexture.Get(), &dsvRODesc, depthStencilViewRO.ReleaseAndGetAddressOf());

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	Renderer::device->CreateShaderResourceView(shadowTexture.Get(), &srvDesc, shadowSRV.ReleaseAndGetAddressOf());
}
