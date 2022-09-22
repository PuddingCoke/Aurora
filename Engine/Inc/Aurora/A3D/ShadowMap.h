#pragma once

#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_

#include<Aurora/Renderer.h>
#include<Aurora/Shader.h>

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

	ID3D11DepthStencilView* getROView() const;

	ID3D11ShaderResourceView* getSRV() const;

	static Shader* shadowVShader;

private:

	friend class Aurora;

	static void ini();

	static void release();

	ShadowMap(const unsigned int& width, const unsigned int& height);

	ComPtr<ID3D11Texture2D> shadowTexture;

	ComPtr<ID3D11DepthStencilView> depthStencilView;

	//read only view
	ComPtr<ID3D11DepthStencilView> depthStencilViewRO;

	ComPtr<ID3D11ShaderResourceView> shadowSRV;
};

#endif // !_SHADOWMAP_H_