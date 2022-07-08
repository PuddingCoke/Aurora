#pragma once

#ifndef _DOUBLERTV_H_
#define _DOUBLERTV_H_

#include"RenderTexture.h"

class DoubleRTV
{
public:

	DoubleRTV(const UINT& width, const UINT& height, const DXGI_FORMAT& format);

	~DoubleRTV();

	RenderTexture* renderTexture1;

	RenderTexture* renderTexture2;

	const unsigned int width;

	const unsigned int height;

	RenderTexture* read();

	void read(RenderTexture* const renderTexture);

	RenderTexture* write();

	void write(RenderTexture* const renderTexture);

	void swap();

};

#endif // !_DOUBLERTV_H_
