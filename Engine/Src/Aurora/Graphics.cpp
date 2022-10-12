#include<Aurora/Graphics.h>

Graphics* Graphics::instance = nullptr;

ID3D11Buffer* Graphics::getDeltaTimeBuffer()
{
	return instance->deltaTimeBuffer.Get();
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

const float& Graphics::getAspectRatio()
{
	return instance->aspectRatio;
}

const int& Graphics::getWidth()
{
	return instance->width;
}

const int& Graphics::getHeight()
{
	return instance->height;
}

const unsigned int& Graphics::getMSAALevel()
{
	return instance->msaaLevel;
}

void Graphics::setRecordConfig(const unsigned int& frameToEncode, const unsigned int& frameRate)
{
	instance->recordConfig.frameToEncode = frameToEncode;
	instance->recordConfig.frameRate = frameRate;
}

Graphics::Graphics(const int& width, const int& height, const unsigned int& msaaLevel) :
	width(width), height(height), msaaLevel(msaaLevel), aspectRatio((float)width / (float)height), fpsCalculator{}, deltaTime{ 0,0,0,0 }, recordConfig{ 1800,60 }
{
	std::cout << "[class Graphics] resolution:" << width << " " << height << "\n";
	std::cout << "[class Graphics] aspectRatio:" << aspectRatio << "\n";
	std::cout << "[class Graphics] multisample level:" << msaaLevel << "\n";

	//初始化和时间相关的Constant Buffer
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(DeltaTime);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Renderer::device->CreateBuffer(&cbd, nullptr, deltaTimeBuffer.ReleaseAndGetAddressOf());
		updateDeltaTimeBuffer();
	}
}

void Graphics::updateDeltaTimeBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(deltaTimeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &deltaTime, sizeof(DeltaTime));
	Renderer::context->Unmap(deltaTimeBuffer.Get(), 0);
}
