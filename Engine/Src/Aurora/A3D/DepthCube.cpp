#include<Aurora/A3D/DepthCube.h>

DepthCube::DepthCube(const UINT& resolution, const DXGI_FORMAT& texFormat, const DXGI_FORMAT& srvFormat, const DXGI_FORMAT& dsvFormat):
	Texture2D(resolution, resolution, 1, 6, texFormat, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_TEXTURECUBE)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = srvFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;
		srvDesc.TextureCube.MostDetailedMip = 0;

		createSRV(texture.Get(), srvDesc);
	}

	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dsvFormat;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.ArraySize = 6;
		dsvDesc.Texture2DArray.FirstArraySlice = 0;
		dsvDesc.Texture2DArray.MipSlice = 0;

		createDSV(texture.Get(), dsvDesc);
	}
}

void DepthCube::bindDSV()
{
	unbindFromSRV();
}

void DepthCube::bindSRV()
{

}
