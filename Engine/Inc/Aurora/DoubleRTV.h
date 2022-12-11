#pragma once

#ifndef _DOUBLERTV_H_
#define _DOUBLERTV_H_

#include"RenderTexture.h"

class DoubleRTV
{
public:

	DoubleRTV() = delete;

	DoubleRTV(const DoubleRTV&) = delete;

	void operator=(const DoubleRTV&) = delete;

	DoubleRTV(const UINT& width, const UINT& height, const DXGI_FORMAT& format);

	~DoubleRTV();

	const unsigned int width;

	const unsigned int height;

	RenderTexture* read() const;

	RenderTexture* write() const;

	void swap();

private:

	RenderTexture* renderTexture1;

	RenderTexture* renderTexture2;

};

#endif // !_DOUBLERTV_H_
