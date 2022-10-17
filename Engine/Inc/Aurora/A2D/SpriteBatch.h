﻿#pragma once

#ifndef _SPRITEBATCH_H_
#define _SPRITEBATCH_H_

#include<vector>

#include<Aurora/A2D/BitmapFont.h>
#include<Aurora/ResourceTexture.h>
#include<Aurora/RenderTexture.h>
#include<Aurora/Shader.h>
#include<Aurora/States.h>
#include<Aurora/ResManager.h>

class SpriteBatch
{
public:

	static SpriteBatch* create();

	static SpriteBatch* get();

	SpriteBatch(const SpriteBatch&) = delete;

	void operator=(const SpriteBatch&) = delete;

	~SpriteBatch();

	void begin();

	void end();

	void draw(ResourceTexture* const texture, const float& x, const float& y);

	void draw(ResourceTexture* const texture, const float& x, const float& y, const float& originX, const float& originY);

	void draw(ResourceTexture* const texture, const float& x, const float& y, const float& originX, const float& originY, const float& rotation);

	void draw(BitmapFont* const font, const std::string& context, const float& x, const float& y, const float& r = 1.f, const float& g = 1.f, const float& b = 1.f, const float& a = 1.f);

	void draw(BitmapFont* const font, const char& context, const float& x, const float& y, const float& r = 1.f, const float& g = 1.f, const float& b = 1.f, const float& a = 1.f);

private:

	friend class BitmapFont;

	static SpriteBatch* instance;

	void compileShaders();

	int texturePoolAdd(ResourceTexture* const texture);

	void flush();

	SpriteBatch();

	ComPtr<ID3D11InputLayout> spriteInputLayout;

	ComPtr<ID3D11InputLayout> bitmapInputLayout;

	ComPtr<ID3D11Buffer> indexBuffer;

	Shader* spriteVShader;
	Shader* spritePShader;

	Shader* bitmapVShader;
	Shader* bitmapPShader;

	static constexpr int maxSpriteNum = 8192;

	class TextureSlot
	{
	public:

		static int curTextureNum;

		static constexpr int maxTextureSlotNum = 32;

		TextureSlot();

		~TextureSlot();

		void updateVertices() const;

		void drawVertices() const;

		void addVertices(const float& x1, const float& y1, const float& x2, const float& y2, const float& x3, const float& y3, const float& x4, const float& y4);

		int idx;

		float* vertices;

		ResourceTexture* texture;

		ComPtr<ID3D11Buffer> vertexBuffer;
	};

	TextureSlot* texturePool;

	std::vector<BitmapFont*> fontPool;
};

#endif // !_SPRITEBATCH_H_