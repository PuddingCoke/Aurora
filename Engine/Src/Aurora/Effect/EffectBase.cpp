#include<Aurora/Effect/EffectBase.h>

EffectBase::EffectBase(const unsigned int& width, const unsigned int& height, const DXGI_FORMAT& outputFormat) :
	outputRTV(new RenderTexture(width, height, outputFormat)), width(width), height(height)
{
}

EffectBase::~EffectBase()
{
	delete outputRTV;
}
