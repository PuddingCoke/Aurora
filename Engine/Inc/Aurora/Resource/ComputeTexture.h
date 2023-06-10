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

	UnorderedAccessView* getUAVMip(const UINT& index);

	ShaderResourceView* getSRVMip(const UINT& index);

	virtual void bindSRV() override;

	void clearUAV(const float* const color, const UINT& index) const;

	void clearUAV(const unsigned int* const value, const UINT& index) const;

private:

	USView* mipArray;

};

#endif // !_COMPUTETEXTURE_H_
