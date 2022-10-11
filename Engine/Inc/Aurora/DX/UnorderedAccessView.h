#pragma once

#ifndef _UNORDEREDACCESSVIEW_H_
#define _UNORDEREDACCESSVIEW_H_

#include<Aurora/Renderer.h>

class UnorderedAccessView
{
public:

	UnorderedAccessView();

	virtual ~UnorderedAccessView();

	UnorderedAccessView(const UnorderedAccessView&) = delete;

	void operator=(const UnorderedAccessView&) = delete;

	void CSSetUAV(const UINT& slot = 0) const;

protected:

	ComPtr<ID3D11UnorderedAccessView> unorderedAccessView;

};

#endif // !_UNORDEREDACCESSVIEW_H_

