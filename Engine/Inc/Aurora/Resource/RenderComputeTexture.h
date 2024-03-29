#pragma once

#ifndef _RCTEXTURE_H_
#define _RCTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/RUSView.h>

class RenderComputeTexture :public Texture2D, public ShaderResourceView
{
public:

	RenderComputeTexture(const UINT& width, const UINT& height, const FMT& fmt, const float color[4], const UINT& mipLevels = 1, const UINT& arraySize = 1);

	RenderComputeTexture(const RenderComputeTexture&);

	virtual ~RenderComputeTexture();

	RUSView* getMip(const UINT& index);

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

private:

	class RUSViewEx :public RUSView
	{
	public:

		ShaderResourceView* allSRV;

		void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

		void bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

		void bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

		void bindRTV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	}*mipArray;

};

#endif // !_RCTEXTURE_H_
