#pragma once

#ifndef _RENDERTEXTURE_H_
#define _RENDERTEXTURE_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>
#include<Aurora/Core/DX/View/Composition/RSView.h>

class RenderTexture :public Texture2D, public ShaderResourceView
{
public:

	RenderTexture(const UINT& width, const UINT& height, const FMT& fmt, const float color[4] = DirectX::Colors::Black, const bool& enableMSAA = false);

	RenderTexture(const UINT& width, const UINT& height, const FMT& fmt, const UINT& mipLevels, const UINT& arraySize, const float color[4] = DirectX::Colors::Black);

	virtual ~RenderTexture();

	RSView* getMip(const UINT& index);

	virtual void bindSRV() override;

	void clearRTV(const float color[4], const UINT& index) const;

private:

	class RSViewEx :public RSView
	{
	public:

		ShaderResourceView* allSRV;

		void bindRTV() override;

		void bindSRV() override;

	}*mipArray;

	//RSView* mipArray;

};

#endif // !_RENDERTEXTURE_H_
