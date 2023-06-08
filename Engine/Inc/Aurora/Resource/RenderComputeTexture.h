#pragma once

#ifndef _RCTEXTURE_H_
#define _RCTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/RUSView.h>

class RenderComputeTexture :public Texture2D, public ShaderResourceView
{
public:

	RenderComputeTexture(const UINT& width, const UINT& height, const DXGI_FORMAT& format, const float color[4], const UINT& mipLevels = 1, const UINT& arraySize = 1);

	virtual ~RenderComputeTexture();

	RenderTargetView* getRTVMip(const UINT& index);

	UnorderedAccessView* getUAVMip(const UINT& index);

	ShaderResourceView* getSRVMip(const UINT& index);

	virtual void bindSRV() override;

	void clearUAV(const float* const color, const UINT& index) const;

	void clearUAV(const unsigned int* const value, const UINT& index) const;

	void clearRTV(const float color[4], const UINT& index) const;

private:

	RUSView* mipArray;

};

#endif // !_RCTEXTURE_H_
