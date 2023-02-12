#pragma once

#ifndef _DEPTHCUBEARRAY_H_
#define _DEPTHCUBEARRAY_H_

#include<vector>

#include<Aurora/DX/Resource/Texture2D.h>
#include<Aurora/DX/View/USView.h>
#include<Aurora/DX/View/DepthStencilView.h>

class DepthCubeArray :public Texture2D, public ShaderResourceView
{
public:

	DepthCubeArray(const UINT& resolution, const UINT& cubeNum, const DXGI_FORMAT& texFormat = DXGI_FORMAT_R32_TYPELESS, const DXGI_FORMAT& srvFormat = DXGI_FORMAT_R32_FLOAT, const DXGI_FORMAT& dsvFormat = DXGI_FORMAT_D32_FLOAT);

	~DepthCubeArray();

	DepthStencilView* getCubeDSV(const UINT& index);

private:

	std::vector<DepthStencilView*> depthStencilViews;

};

#endif // !_DEPTHCUBEARRAY_H_
