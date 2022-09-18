#include<Aurora/PostProcessing/EffectBase.h>

EffectBase::EffectBase(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& outputFormat) :
	outputRTV(RenderTexture::create(width, height, outputFormat)), width(width), height(height)
{
}

EffectBase::~EffectBase()
{
	delete outputRTV;
}

Texture2D* EffectBase::getOutputTexture() const
{
	return outputRTV->getTexture();
}
