#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include"Renderer.h"

class Graphics
{
public:

	Graphics() = delete;

	Graphics(const Graphics&) = delete;

	void operator=(const Graphics&) = delete;

	static ID3D11Buffer* getDeltaTimeBuffer();

	static const float& getDeltaTime();

	static const float& getSTime();

	static const float& getFPS();

	static const float& getAspectRatio();

	static const int& getWidth();

	static const int& getHeight();

	static unsigned int& getMSAALevel();

private:

	friend class Aurora;

	friend class WallpaperHelper;

	friend class NvidiaEncoder;

	static void ini();

	static void updateDeltaTimeBuffer();

	static ComPtr<ID3D11Buffer> deltaTimeBuffer;

	static ComPtr<ID3D11Debug> d3dDebug;

	static struct DeltaTime
	{
		float deltaTime = 0;
		float sTime = 0;
		float v2 = 0;
		float v3 = 0;
	} deltaTime;

	static float frameDuration;

	static int frameCount;

	static float framePerSec;

	static int width;

	static int height;

	static float aspectRatio;

	static unsigned int msaaLevel;

};

#endif // !_GRAPHICS_H_
