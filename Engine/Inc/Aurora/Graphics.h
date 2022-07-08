#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include<d3d11_4.h>
#include<wrl/client.h>
#include<DirectXMath.h>


using Microsoft::WRL::ComPtr;

class Graphics
{
public:

	static ComPtr<ID3D11Device4> device;

	static ComPtr<ID3D11DeviceContext4> context;

	static void setDefRTV(ID3D11DepthStencilView* const view = nullptr);

	static void clearDefRTV(const float color[4]);

	static const float& getDeltaTime();

	static const float& getFPS();

	static void setView(const DirectX::XMMATRIX& view);

	static const int& getWidth();

	static const int& getHeight();

	static void setViewport(const float& vWidth, const float& vHeight);

private:

	friend class Aurora;

	friend class RenderTexture;

	static void setProj(const DirectX::XMMATRIX& proj);

	static ComPtr<ID3D11Buffer> cBufferProj;//投影矩阵通常在窗口大小变化时更改

	static ComPtr<ID3D11Buffer> cBufferView;//视图矩阵可由用户随意修改

	static ComPtr<ID3D11RenderTargetView> defaultTargetView;

	static float deltaTime;

	static float frameDuration;

	static int frameCount;

	static float framePerSec;

	static int width;

	static int height;

	static float aspectRatio;

	static unsigned int msaaLevel;

	static D3D11_VIEWPORT vp;

};

#endif // !_GRAPHICS_H_
