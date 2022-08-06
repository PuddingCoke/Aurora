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

RenderTexture* DoubleRTV::read() const
{
	return renderTexture1;
}

RenderTexture* DoubleRTV::write() const
{
	return renderTexture2;
}

void DoubleRTV::swap()
{
	RenderTexture* const temp = renderTexture1;
	renderTexture1 = renderTexture2;
	renderTexture2 = temp;
}
