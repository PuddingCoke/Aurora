#pragma once

#include<Aurora/Shader.h>
#include<Aurora/PostProcessing/EffectBase.h>

class DualKawaseBlur :public EffectBase
{
public:

	DualKawaseBlur(const unsigned int& width, const unsigned int& height, const float& pixelOffset = 1.5f);

	~DualKawaseBlur();

	Shader* downSampleShader;

	Shader* upSampleShader;

	Shader* bloomFinal;

	static constexpr int iterations = 5;

	DirectX::XMUINT2 resolutions[iterations];

	RenderTexture* renderTextures[iterations];

	RenderTexture* bloomTexture;

	//DYNAMIC 16bytes
	ComPtr<ID3D11Buffer> offsetBuffer;

	//IMMUTABLE 16bytes
	ComPtr<ID3D11Buffer> halfPixelBuffer[iterations];

	ComPtr<ID3D11Buffer> screenHalfPixelBuffer;

	Texture2D* process(Texture2D* const texture2D) const override;

	const float& getPixelOffset() const;

	void setPixelOffset(const float& pixelOffset);

	//越高图像越模糊
	float pixelOffset;

private:

	void compileShaders() override;

};