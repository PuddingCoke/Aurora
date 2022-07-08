#include<Aurora/PostProcessing/EffectBase.h>

EffectBase::EffectBase(const unsigned int& width, const unsigned int& height) :
	defRenderTexture(RenderTexture::create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM)), width(width), height(height)
{
	outputTexture = defRenderTexture->getTexture();
}

EffectBase::~EffectBase()
{
	delete defRenderTexture;
}
