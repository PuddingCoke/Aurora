#pragma once

#ifndef _COMPUTETEXTURE3D_H_
#define _COMPUTETEXTURE3D_H_

#include<Aurora/Core/DX/Resource/Texture3D.h>
#include<Aurora/Core/DX/View/Composition/USView.h>

class ComputeTexture3D :public Texture3D, public ShaderResourceView
{
public:

	ComputeTexture3D(const UINT& width, const UINT& height, const UINT& depth, const FMT& resFmt, const FMT& srvFmt, const FMT& uavFmt, const UINT& extraBindFlags = 0, const UINT& miscFlags = 0, const UINT& mipLevels = 1);

	ComputeTexture3D(const ComputeTexture3D&);

	virtual ~ComputeTexture3D();

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

};

#endif // !_COMPUTETEXTURE3D_H_
