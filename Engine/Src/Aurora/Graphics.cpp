#include<Aurora/Graphics.h>

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

float Graphics::getFPS()
{
	instance->fpsCalculator.frameTime[instance->fpsCalculator.frameTimeIndex] = instance->deltaTime.deltaTime;

	instance->fpsCalculator.frameTimeIndex = (instance->fpsCalculator.frameTimeIndex + 1) % 20;

	float sumTime = 0;

	for (int i = 0; i < 20; i++)
	{
		sumTime += instance->fpsCalculator.frameTime[i];
	}

	return 20.f / sumTime;
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
	width(width), height(height), msaaLevel(msaaLevel), aspectRatio((float)width / (float)height), fpsCalculator{}, deltaTime{ 0,0,0,0 }, recordConfig{ 1800,60 },
	deltaTimeBuffer(new Buffer(sizeof(DeltaTime),D3D11_BIND_CONSTANT_BUFFER,D3D11_USAGE_DYNAMIC,nullptr,D3D11_CPU_ACCESS_WRITE))
{
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
	memcpy(deltaTimeBuffer->map(0).pData, &deltaTime, sizeof(DeltaTime));
	deltaTimeBuffer->unmap(0);
}
