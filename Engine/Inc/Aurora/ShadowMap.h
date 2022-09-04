#pragma once

#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_

#include"Renderer.h"

class ShadowMap
{
public:

	ShadowMap() = delete;

	ShadowMap(const ShadowMap&) = delete;

	void operator=(const ShadowMap&) = delete;

	static ShadowMap* create(const unsigned int& width, const unsigned int& height);

	void setSRV(const UINT& slot) const;

	void clear(const float& depth = 1.0f) const;

	ID3D11DepthStencilView* get() const;

private:

	ShadowMap(const unsigned int& width, const unsigned int& height);

	ComPtr<ID3D11Texture2D> shadowTexture;

	ComPtr<ID3D11DepthStencilView> depthStencilView;

	ComPtr<ID3D11ShaderResourceView> shadowSRV;

};

#endif // !_SHADOWMAP_H_