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

	static const float& getSTime();

	static const float& getFPS();

	static const float& getAspectRatio();

	static void setView(const DirectX::XMMATRIX& view);

	static const int& getWidth();

	static const int& getHeight();

	static unsigned int& getMSAALevel();

	static void setViewport(const float& vWidth, const float& vHeight);

	static void setPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY& topology);

	static void setBlendState(ID3D11BlendState* const blendState);

private:

	friend class Aurora;

	friend class NvidiaEncoder;

	static void setProj(const DirectX::XMMATRIX& proj);

	static void updateGPUDeltaTimes();

	static ComPtr<ID3D11Buffer> cBufferProj;//投影矩阵通常在窗口大小变化时更改

	static ComPtr<ID3D11Buffer> cBufferView;//视图矩阵可由用户随意修改

	static ComPtr<ID3D11Buffer> cBufferDeltaTimes;

	static ComPtr<ID3D11Debug> d3dDebug;

	//R8G8B8A8_UNORM
	static ComPtr<ID3D11Texture2D> encodeTexture;

	static ID3D11Texture2D* backBuffer;

	static struct GPUDeltaTimes
	{
		float deltaTime = 0;
		float sTime = 0;
		float v2 = 0;
		float v3 = 0;
	} gpuDeltaTimes;

	static ComPtr<ID3D11RenderTargetView> defaultTargetView;

	static float deltaTime;

	static float sTime;

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
