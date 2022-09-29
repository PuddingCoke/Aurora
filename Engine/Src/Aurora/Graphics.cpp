#include<Aurora/Graphics.h>

ComPtr<ID3D11Buffer> Graphics::deltaTimeBuffer;

ComPtr<ID3D11Debug> Graphics::d3dDebug;

//默认状态60帧录制30秒
Graphics::RecordConfig Graphics::recordConfig = { 1800,60 };

Graphics::DeltaTime Graphics::deltaTime = { 0,0,0,0 };

float Graphics::frameDuration = 0;

int Graphics::frameCount = 0;

float Graphics::framePerSec = 0;

int Graphics::width = 0;

int Graphics::height = 0;

float Graphics::aspectRatio = 0;

unsigned int Graphics::msaaLevel = 0;

ID3D11Buffer* Graphics::getDeltaTimeBuffer()
{
	return deltaTimeBuffer.Get();
}

const float& Graphics::getDeltaTime()
{
	return deltaTime.deltaTime;
}

const float& Graphics::getSTime()
{
	return deltaTime.sTime;
}

const float& Graphics::getFPS()
{
	frameDuration += deltaTime.deltaTime;
	frameCount++;
	if (frameDuration > .1f)
	{
		framePerSec = frameCount / frameDuration;
		frameCount = 0;
		frameDuration = 0;
	}
	return framePerSec;
}

const float& Graphics::getAspectRatio()
{
	return aspectRatio;
}

const int& Graphics::getWidth()
{
	return width;
}

const int& Graphics::getHeight()
{
	return height;
}

unsigned int& Graphics::getMSAALevel()
{
	return msaaLevel;
}

void Graphics::setRecordConfig(const unsigned int& frameToEncode, const unsigned int& frameRate)
{
	recordConfig.frameToEncode = frameToEncode;
	recordConfig.frameRate = frameRate;
}

void Graphics::ini()
{
	//初始化和时间相关的Constant Buffer
	{
		D3D11_BUFFER_DESC cbd = {};
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.ByteWidth = sizeof(DeltaTime);
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		Renderer::device->CreateBuffer(&cbd, nullptr, Graphics::deltaTimeBuffer.ReleaseAndGetAddressOf());
		Graphics::updateDeltaTimeBuffer();
	}
}

void Graphics::updateDeltaTimeBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedData;
	Renderer::context->Map(deltaTimeBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, &deltaTime, sizeof(DeltaTime));
	Renderer::context->Unmap(deltaTimeBuffer.Get(), 0);
}
