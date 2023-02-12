#include<Aurora/A3D/RenderCube.h>

RenderCube::RenderCube(const UINT& resolution, const DXGI_FORMAT& format, const float color[4]) :
	resolution(resolution)
{
	{
		D3D11_TEXTURE2D_DESC desc = {};
		desc.Width = resolution;
		desc.Height = resolution;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.Format = format;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.ArraySize = 6;
		desc.MipLevels = 1;
		Renderer::device->CreateTexture2D(&desc, nullptr, cubeTexture.ReleaseAndGetAddressOf());
	}

	{
		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.MipSlice = 0;
		createRTV(cubeTexture.Get(), desc);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = 1;
		desc.TextureCube.MostDetailedMip = 0;
		createSRV(cubeTexture.Get(), desc);
	}

	clearRTV(color);
}

RenderCube::~RenderCube()
{
	
}
