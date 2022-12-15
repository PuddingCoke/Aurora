#pragma once

#ifndef _GRAPHICS_H_
#define _GRAPHICS_H_

#include"DX/Resource/Buffer.h"
#include"Random.h"

class Graphics
{
public:

	Graphics() = delete;

	Graphics(const Graphics&) = delete;

	void operator=(const Graphics&) = delete;

	static Buffer* getDeltaTimeBuffer();

	static const float& getDeltaTime();

	static const float& getSTime();

	static void setRecordConfig(const unsigned int& frameToEncode, const unsigned int& frameRate);

	static const int& getWidth();

	static const int& getHeight();

	static const float& getAspectRatio();

	static const unsigned int& getMSAALevel();

private:

	friend class Aurora;

	static Graphics* instance;

	Graphics(const int& width, const int& height, const unsigned int& msaaLevel);

	~Graphics();

	void updateDeltaTimeBuffer();

	Buffer* deltaTimeBuffer;

	struct RecordConfig
	{
		unsigned int frameToEncode;
		unsigned int frameRate;
	} recordConfig;

	struct DeltaTime
	{
		float deltaTime = 0;
		float sTime = 0;
		unsigned int uintSeed = 0;
		float floatSeed = 0;
	} deltaTime;

	const int width;

	const int height;

	const float aspectRatio;

	const unsigned int msaaLevel;

};

#endif // !_GRAPHICS_H_
