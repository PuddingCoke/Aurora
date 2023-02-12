#include<Aurora/DepthCubeArray.h>

DepthCubeArray::DepthCubeArray(const UINT& resolution, const UINT& cubeNum, const DXGI_FORMAT& texFormat, const DXGI_FORMAT& srvFormat, const DXGI_FORMAT& dsvFormat):
	Texture2D(resolution, resolution, 1, cubeNum * 6, texFormat, D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE, D3D11_RESOURCE_MISC_TEXTURECUBE)
{
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = srvFormat;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
		srvDesc.TextureCubeArray.First2DArrayFace = 0;
		srvDesc.TextureCubeArray.MipLevels = 1;
		srvDesc.TextureCubeArray.MostDetailedMip = 0;
		srvDesc.TextureCubeArray.NumCubes = cubeNum;

		createSRV(texture.Get(), srvDesc);
	}

	for (UINT i = 0; i < cubeNum; i++)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = dsvFormat;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.ArraySize = 6;
		dsvDesc.Texture2DArray.FirstArraySlice = 6 * i;
		dsvDesc.Texture2DArray.MipSlice = 0;

		depthStencilViews.push_back(new DepthStencilView(texture.Get(), dsvDesc));
	}

}

DepthCubeArray::~DepthCubeArray()
{
	for (UINT i = 0; i < depthStencilViews.size(); i++)
	{
		delete depthStencilViews[i];
	}
}

DepthStencilView* DepthCubeArray::getCubeDSV(const UINT& index)
{
	unbindFromSRV();
	return depthStencilViews[index];
}
