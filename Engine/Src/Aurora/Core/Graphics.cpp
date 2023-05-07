#include<Aurora/Core/Graphics.h>

Graphics* Graphics::instance = nullptr;

Buffer* Graphics::getDeltaTimeBuffer()
{
	return instance->deltaTimeBuffer;
}

const float& Graphics::getDeltaTime()
{
	return instance->deltaTime.deltaTime;
}

const float& Graphics::getSTime()
{
	return instance->deltaTime.sTime;
}

void Graphics::setRecordConfig(const unsigned int& frameToEncode, const unsigned int& frameRate)
{
	instance->recordConfig.frameToEncode = frameToEncode;
	instance->recordConfig.frameRate = frameRate;
}

const int& Graphics::getWidth()
{
	return instance->width;
}

const int& Graphics::getHeight()
{
	return instance->height;
}

const float& Graphics::getAspectRatio()
{
	return instance->aspectRatio;
}

const unsigned int& Graphics::getMSAALevel()
{
	return instance->msaaLevel;
}

Graphics::Graphics(const int& width, const int& height, const unsigned int& msaaLevel) :
	width(width), height(height), msaaLevel(msaaLevel), aspectRatio((float)width / (float)height), deltaTime{ 0,0,0,0 }, recordConfig{ 3600,60 },
	deltaTimeBuffer(new PerframeCB(sizeof(DeltaTime)))
{
	instance = this;

	std::cout << "[class Graphics] resolution:" << width << " " << height << "\n";
	std::cout << "[class Graphics] aspectRatio:" << aspectRatio << "\n";
	std::cout << "[class Graphics] multisample level:" << msaaLevel << "\n";
}

Graphics::~Graphics()
{
	delete deltaTimeBuffer;
}

void Graphics::updateDeltaTimeBuffer()
{
	deltaTime.uintSeed = Random::Uint();
	deltaTime.floatSeed = Random::Float();
	PerframeCB::updateData(deltaTimeBuffer, &deltaTime, sizeof(DeltaTime));
}
