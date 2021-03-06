#include<Aurora/DoubleRTV.h>

DoubleRTV::DoubleRTV(const UINT& width, const UINT& height, const DXGI_FORMAT& format) :
	width(width), height(height),
	renderTexture1(RenderTexture::create(width, height, format, DirectX::Colors::Transparent, false)),
	renderTexture2(RenderTexture::create(width, height, format, DirectX::Colors::Transparent, false))
{
}

DoubleRTV::~DoubleRTV()
{
	delete renderTexture1;
	delete renderTexture2;
}

RenderTexture* DoubleRTV::read()
{
	return renderTexture1;
}

void DoubleRTV::read(RenderTexture* const renderTexture)
{
	delete renderTexture1;
	renderTexture1 = renderTexture;
}

RenderTexture* DoubleRTV::write()
{
	return renderTexture2;
}

void DoubleRTV::write(RenderTexture* const renderTexture)
{
	delete renderTexture2;
	renderTexture2 = renderTexture;
}

void DoubleRTV::swap()
{
	RenderTexture* const temp = renderTexture1;
	renderTexture1 = renderTexture2;
	renderTexture2 = temp;
}
