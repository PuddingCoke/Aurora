#pragma once

#ifndef _RENDERCUBE_H_
#define _RENDERCUBE_H_

#include<DirectxColors.h>

#include<Aurora/DX/View/DepthStencilView.h>
#include<Aurora/DX/View/RSView.h>

class RenderCube :public RSView
{
public:

	RenderCube() = delete;

	RenderCube(const RenderCube&) = delete;

	void operator=(const RenderCube&) = delete;

	RenderCube(const UINT& resolution, const DXGI_FORMAT& format, const float color[4] = DirectX::Colors::Black);

	~RenderCube();

	const UINT resolution;

private:

	ComPtr<ID3D11Texture2D> cubeTexture;

};

#endif // !_RENDERCUBE_H_
