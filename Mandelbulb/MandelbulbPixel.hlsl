Texture3D tTexture : register(t0);

SamplerState linearSampler : register(s0);

float4 main() : SV_TARGET
{
	tTexture.Sample()
	
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}