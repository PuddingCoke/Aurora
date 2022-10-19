#pragma once

#ifndef _SHADOWMAP_H_
#define _SHADOWMAP_H_

#include<Aurora/Renderer.h>
#include<Aurora/Shader.h>
#include<Aurora/DX/View/ShaderResourceView.h>

class ShadowMap :public ShaderResourceView
{
public:

	ShadowMap() = delete;

	ShadowMap(const ShadowMap&) = delete;

	void operator=(const ShadowMap&) = delete;

	static ShadowMap* create(const unsigned int& width, const unsigned int& height);

	void clear(const float& depth = 1.0f) const;

	ID3D11DepthStencilView* get() const;

	ID3D11DepthStencilView* getROView() const;

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

};

#endif // !_SHADOWMAP_H_