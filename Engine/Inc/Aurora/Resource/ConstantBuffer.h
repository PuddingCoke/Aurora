#pragma once

#ifndef _CONSTANTBUFFER_H_
#define _CONSTANTBUFFER_H_

#include<Aurora/Core/DX/Resource/Buffer.h>

class ConstantBuffer :public Buffer
{
public:

	ConstantBuffer(const UINT& byteWidth, const D3D11_USAGE& usage,
		const void* const data = nullptr, const UINT& miscFlags = 0);

};

#endif // !_CONSTANTBUFFER_H_
