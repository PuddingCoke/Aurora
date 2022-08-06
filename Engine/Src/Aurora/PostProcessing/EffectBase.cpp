#include<Aurora/PostProcessing/EffectBase.h>

EffectBase::EffectBase(const unsigned int& width, const unsigned int& height) :
	defRenderTexture(RenderTexture::create(width, height, DXGI_FORMAT_R8G8B8A8_UNORM)), width(width), height(height)
{
	outputTexture = defRenderTexture->getTexture();
}

EffectBase::EffectBase(RenderTexture* defRenderTexture) :
	defRenderTexture(defRenderTexture), width(defRenderTexture->width), height(defRenderTexture->height), outputTexture(nullptr)
{
}

EffectBase::~EffectBase()
{
	if (outputTexture)
	{
		delete defRenderTexture;
	}
}

Texture2D* EffectBase::process(Texture2D* const texture2D) const
{
	return texture2D;
}

void EffectBase::process() const
{
}

Texture2D* EffectBase::getOutputTexture() const
{
	return outputTexture;
}
