#pragma once

#ifndef _COMPUTETEXTURE_H_
#define _COMPUTETEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/USView.h>

class ComputeTexture :public Texture2D, public ShaderResourceView
{
public:

	ComputeTexture(const UINT& width, const UINT& height, const FMT& resFmt, const FMT& srvFmt, const FMT& uavFmt, const UINT& mipLevels = 1, const UINT& arraySize = 1);

	virtual ~ComputeTexture();

	USView* getMip(const UINT& index);

	virtual void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

private:

	class USViewEx :public USView
	{
	public:

		ShaderResourceView* allSRV;

		void bindSRV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

		void bindCUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

		void bindPUAV(ID3D11DeviceContext3* const ctx, GraphicsStates* const states) override;

	}*mipArray;

	//USView* mipArray;

};

#endif // !_COMPUTETEXTURE_H_
