#pragma once

#ifndef _CTEXTUREWITHMIPS_H_
#define _CTEXTUREWITHMIPS_H_

#include<Aurora/DX/Resource/Texture2D.h>
#include<Aurora/DX/View/ShaderResourceView.h>
#include<Aurora/DX/View/UnorderedAccessView.h>

class CTextureWithMips :public Texture2D, public ShaderResourceView
{
public:

	class CustomedUAV :public UnorderedAccessView
	{
	public:

		CustomedUAV();

		void bindCUAV() override;

		void bindPUAV() override;

	};

	CTextureWithMips(const UINT& width, const UINT& height, const DXGI_FORMAT& format, const UINT& mipLevels, const UINT& arraySize = 1);

	virtual ~CTextureWithMips();

	void bindSRV() override;

	UnorderedAccessView* getUAV(const UINT& mipSlice);

private:

	CustomedUAV* uav;

};

#endif // !_CTEXTUREWITHMIPS_H_
