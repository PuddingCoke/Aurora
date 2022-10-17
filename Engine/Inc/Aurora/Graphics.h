#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include"DX/Resource/Buffer.h"

class Graphics
{
public:

	Graphics() = delete;

	Graphics(const Graphics&) = delete;

	void operator=(const Graphics&) = delete;

	static ID3D11Buffer* getDeltaTimeBuffer();

	static const float& getDeltaTime();

	static const float& getSTime();

	static float getFPS();

	static const float& getAspectRatio();

	static const int& getWidth();

	static const int& getHeight();

	static const unsigned int& getMSAALevel();

	static void setRecordConfig(const unsigned int& frameToEncode, const unsigned int& frameRate);

private:

	friend class Aurora;

	static Graphics* instance;

	Graphics(const int& width, const int& height, const unsigned int& msaaLevel);

	void updateDeltaTimeBuffer();

	ComPtr<ID3D11Buffer> deltaTimeBuffer;

	struct RecordConfig
	{
		unsigned int frameToEncode;
		unsigned int frameRate;
	} recordConfig;

	struct DeltaTime
	{
		float deltaTime = 0;
		float sTime = 0;
		float v2 = 0;
		float v3 = 0;
	} deltaTime;

	struct FPSCalculator
	{
		float frameTime[20];
		int frameTimeIndex;
	} fpsCalculator;

	const int width;

	const int height;

	const float aspectRatio;

	const unsigned int msaaLevel;

};

#endif // !_GRAPHICS_H_
