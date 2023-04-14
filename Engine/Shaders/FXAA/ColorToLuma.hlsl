Texture2D<float4> originTexture : register(t0);

SamplerState clampSampler : register(s0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 color = originTexture.Sample(clampSampler, texCoord);
    
    color.a = dot(color.rgb, float3(0.299, 0.587, 0.114));
    
    return color;
}