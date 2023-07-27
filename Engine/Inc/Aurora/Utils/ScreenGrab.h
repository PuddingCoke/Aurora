#pragma once

#ifndef _SCREENGRAB_H_
#define _SCREENGRAB_H_

#include<Aurora/Core/DX/Resource/Texture2D.h>

#include<Aurora/EngineAPI/Graphics.h>

#include<Aurora/Input/Keyboard.h>

#include<stb_image/stb_image_write.h>

class ScreenGrab
{
public:

	ScreenGrab(const ScreenGrab&) = delete;

	void operator=(const ScreenGrab&) = delete;

private:

	friend class Aurora;

	ScreenGrab(ComPtr<ID3D11Texture2D> backBuffer);

	~ScreenGrab();

	Texture2D* copyTexture;

	Texture2D* backBuffer;

	int eventIndex;

};

#endif // !_SCREENGRAB_H_
