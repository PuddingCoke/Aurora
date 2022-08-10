#pragma once

#ifndef _BITMAPFONT_H_
#define _BITMAPFONT_H_

#include<string>
#include<unordered_map>
#include<fstream>

#include<Aurora/Texture2D.h>

class BitmapFont
{
public:

	static BitmapFont* create(const std::string& bitmapPath, const std::string& configFilePath, const int& fontSize = -1);

	BitmapFont() = delete;

	BitmapFont(const BitmapFont&) = delete;

	void operator=(const BitmapFont&) = delete;

	~BitmapFont();

	struct Character
	{
		int xoffset;
		int yoffset;
		int xadvance;

		float width;
		float height;

		float leftTexCoord;
		float rightTexCoord;
		float bottomTexCoord;
		float topTexCoord;
	};

	const Character& getCharacter(const char& c);

	const int& getFontSize() const;

	void setFontSize(const int& fontSize);

	const float& getScale() const;

private:

	friend class SpriteBatch;

	BitmapFont(const std::string& bitmapPath, const std::string& configFilePath, const int& fontSize);

	void updateVerticesData() const;

	void render();

	int fontSize;

	int originFontSize;

	float scale;

	Texture2D* const texture2D;

	ComPtr<ID3D11Buffer> vertexBuffer;

	float* vertices;

	int idx;

	static constexpr int maxCharacterCount = 8192;

	std::unordered_map<char, const Character> characterMap;

};

#endif // !_BITMAPFONT_H_
