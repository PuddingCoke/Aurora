#include<Aurora/Effect/HBAOEffect.h>

HBAOEffect::HBAOEffect(const unsigned int& width, const unsigned int& height) :
	EffectBase(width, height, FMT::R32F), pAOContext(nullptr)
{
	radius = 2.f;
	bias = 0.1f;
	powerExponent = 2.f;
	blurSharpness = 16.f;

	customHeap.new_ = ::operator new;
	customHeap.delete_ = ::operator delete;

	GFSDK_SSAO_Status status;
	status = GFSDK_SSAO_CreateContext_D3D11(Renderer::getDevice(), &pAOContext, &customHeap);

	if (status == GFSDK_SSAO_OK)
	{
		std::cout << "[class HBAOEffect] initialize pAOContext successfully!\n";
	}
}

HBAOEffect::~HBAOEffect()
{
	pAOContext->Release();
}

ShaderResourceView* HBAOEffect::process(ID3D11ShaderResourceView* const depthSRV, ID3D11ShaderResourceView* const normalSRV) const
{
	outputRTV->unbindFromSRV(ImCtx::GetContext());

	GFSDK_SSAO_InputData_D3D11 input;
	input.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
	input.DepthData.pFullResDepthTextureSRV = depthSRV;
	input.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4((float*)&Camera::getProj());
	input.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
	input.DepthData.MetersToViewSpaceUnits = 1.f;

	input.NormalData.Enable = true;
	input.NormalData.pFullResNormalTextureSRV = normalSRV;
	input.NormalData.WorldToViewMatrix.Data = GFSDK_SSAO_Float4x4((float*)&Camera::getView());
	input.NormalData.WorldToViewMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;

	input.DepthData.Viewport.Enable = true;
	input.DepthData.Viewport.Width = width;
	input.DepthData.Viewport.Height = height;
	input.DepthData.Viewport.MaxDepth = 1.f;
	input.DepthData.Viewport.MinDepth = 0.f;
	input.DepthData.Viewport.TopLeftX = 0;
	input.DepthData.Viewport.TopLeftY = 0;

	GFSDK_SSAO_Parameters params;
	params.DepthStorage = GFSDK_SSAO_FP32_VIEW_DEPTHS;
	params.Radius = radius;
	params.Bias = bias;
	params.PowerExponent = powerExponent;
	params.Blur.Enable = true;
	params.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;
	params.Blur.Sharpness = blurSharpness;
	params.EnableDualLayerAO = false;

	GFSDK_SSAO_Output_D3D11 output;
	output.pRenderTargetView = outputRTV->getMip(0)->getRTV();
	output.Blend.Mode = GFSDK_SSAO_OVERWRITE_RGB;

	pAOContext->RenderAO(ImCtx::GetContext(), input, params, output);

	return outputRTV;
}

const float& HBAOEffect::getRadius() const
{
	return radius;
}

const float& HBAOEffect::getBias() const
{
	return bias;
}

const float& HBAOEffect::getPowerExponent() const
{
	return powerExponent;
}

const float& HBAOEffect::getBlurSharpness() const
{
	return blurSharpness;
}

void HBAOEffect::setRadius(const float& radius)
{
	this->radius = radius;
}

void HBAOEffect::setBias(const float& bias)
{
	this->bias = Math::clamp(bias, 0.f, 0.5f);
}

void HBAOEffect::setPowerExponent(const float& powerExponent)
{
	this->powerExponent = Math::clamp(powerExponent, 1.f, 4.f);
}

void HBAOEffect::setBlurSharpness(const float& blurSharpness)
{
	this->blurSharpness = Math::clamp(blurSharpness, 0.f, 16.f);
}

void HBAOEffect::compileShaders()
{
}
