#pragma once

#ifndef _SPRITEBATCH_H_
#define _SPRITEBATCH_H_

#include<vector>

#include"BitmapFont.h"
#include"Texture2D.h"
#include"RenderTexture.h"
#include"Shader.h"

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

	void draw(Texture2D* const texture, const float& x, const float& y);

	void draw(Texture2D* const texture, const float& x, const float& y, const float& originX, const float& originY);

	void draw(Texture2D* const texture, const float& x, const float& y, const float& originX, const float& originY, const float& rotation);

	void draw(RenderTexture* const renderTexture, const float& x, const float& y);

	void draw(RenderTexture* const renderTexture, const float& x, const float& y, const float& originX, const float& originY);

	void draw(RenderTexture* const renderTexture, const float& x, const float& y, const float& originX, const float& originY, const float& rotation);

	void draw(BitmapFont* const font, const std::string& context, const float& x, const float& y, const float& r = 1.f, const float& g = 1.f, const float& b = 1.f, const float& a = 1.f);

	void draw(BitmapFont* const font, const char& context, const float& x, const float& y, const float& r = 1.f, const float& g = 1.f, const float& b = 1.f, const float& a = 1.f);

private:

	friend class BitmapFont;

	static SpriteBatch* instance;

	void compileShaders();

	void texturePoolAdd(Texture2D* const texture);

	void flush();

	SpriteBatch();

	ComPtr<ID3D11InputLayout> spriteInputLayout;

	ComPtr<ID3D11InputLayout> bitmapInputLayout;

	ComPtr<ID3D11Buffer> indexBuffer;

	ComPtr<ID3D11SamplerState> samplerState;

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

		void setTexture(Texture2D* const texture);

		void updateVertices() const;

		void drawVertices() const;

		void addVertices(const float& x1, const float& y1, const float& x2, const float& y2, const float& x3, const float& y3, const float& x4, const float& y4);

		void reset();

		int idx;

		float* vertices;

		Texture2D* texture;

		ComPtr<ID3D11Buffer> vertexBuffer;
	};

	TextureSlot* texturePool;

	std::vector<BitmapFont*> fontPool;
};

#endif // !_SPRITEBATCH_H_