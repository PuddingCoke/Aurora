#include<Aurora/Effect/EffectBase.h>

EffectBase::EffectBase(GraphicsContext* ctx, const UINT& width, const UINT& height, const FMT& outputFormat) :
	outputRTV(new RenderTexture(width, height, outputFormat)), width(width), height(height), ctx(ctx)
{
}

EffectBase::~EffectBase()
{
	delete outputRTV;
}
