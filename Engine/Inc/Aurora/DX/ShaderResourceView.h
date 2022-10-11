#pragma once

#ifndef _SHADERRESOURCEVIEW_H_
#define _SHADERRESOURCEVIEW_H_

#include<Aurora/Renderer.h>

class ShaderResourceView
{
public:

	ShaderResourceView();

	virtual ~ShaderResourceView();

	ShaderResourceView(const ShaderResourceView&) = delete;

	void operator=(const ShaderResourceView&) = delete;

	void VSSetSRV(const unsigned int& slot = 0) const;

	void GSSetSRV(const unsigned int& slot = 0) const;

	void PSSetSRV(const unsigned int& slot = 0) const;

	void CSSetSRV(const unsigned int& slot = 0) const;

	void DSSetSRV(const unsigned int& slot = 0) const;

	void HSSetSRV(const unsigned int& slot = 0) const;

	ID3D11ShaderResourceView* getSRV() const;

protected:

	ComPtr<ID3D11ShaderResourceView> shaderResourceView;

};

#endif // !_SHADERRESOURCEVIEW_H_
