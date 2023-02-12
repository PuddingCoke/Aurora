#pragma once

#ifndef _COMPUTETEXTUREARRAY_H_
#define _COMPUTETEXTUREARRAY_H_

#include<Aurora/DX/Resource/Texture2D.h>
#include<Aurora/DX/View/USView.h>

class ComputeTextureArray :public Texture2D, public USView
{
public:

	ComputeTextureArray(const UINT& width, const UINT& height, const DXGI_FORMAT& format, const UINT& arraySize);

};

#endif // !_COMPUTETEXTUREARRAY_H_
