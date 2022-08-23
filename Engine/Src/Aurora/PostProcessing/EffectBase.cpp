#include<Aurora/PostProcessing/EffectBase.h>

EffectBase::EffectBase(const unsigned int& width, const unsigned int& height) :
	outputRTV(RenderTexture::create(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT)), width(width), height(height)
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
