#pragma once

#ifndef _PERFRAMECB_H_
#define _PERFRAMECB_H_

#include<Aurora/Core/DX/Resource/Buffer.h>

class PerframeCB :public Buffer
{
public:

	PerframeCB(const UINT& byteWidth);

	static void updateData(PerframeCB* const perframeCB, const void* const data, const size_t& size);

private:

	PerframeCB() = delete;

	PerframeCB(const PerframeCB&) = delete;

	void operator=(const PerframeCB&) = delete;

	friend class Aurora;

	static void mapPerframeBuffer();

	static void unmapPerframeBuffer();

	static void ini();

	static void release();

	static ComPtr<ID3D11Buffer> globalBuffer;//储存每帧需要的所有变量

	static void* pData;

	static UINT curStartConstant;

};

#endif // !_PERFRAMECB_H_