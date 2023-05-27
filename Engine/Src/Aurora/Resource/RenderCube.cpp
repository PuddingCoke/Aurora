#include<Aurora/Resource/RenderCube.h>

RenderCube::RenderCube(const UINT& resolution, const DXGI_FORMAT& format, const float color[4]) :
	Texture2D(resolution, resolution, 1, 6, format, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_TEXTURECUBE)
{
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.Format = format;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;
		desc.Texture2DArray.FirstArraySlice = 0;
		desc.Texture2DArray.MipSlice = 0;
		createRTV(texture.Get(), desc);
	}

	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		desc.TextureCube.MipLevels = 1;
		desc.TextureCube.MostDetailedMip = 0;
		createSRV(texture.Get(), desc);
	}

	clearRTV(color);
}

RenderCube::~RenderCube()
{

}
