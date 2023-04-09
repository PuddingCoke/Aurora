Texture2D reflectedUV : register(t0);
Texture2D originTexture : register(t1);
Texture2D gNormalSpecular : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerComparisonState shadowSampler : register(s2);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float4 uv = reflectedUV.Sample(clampSampler, texCoord);
    
    float3 color = originTexture.Sample(clampSampler, texCoord).rgb;
    
    float alpha = clamp(uv.b, 0.0, 1.0);
        
    color += lerp(float3(0.0, 0.0, 0.0), originTexture.Sample(clampSampler, uv.xy).rgb, alpha) * gNormalSpecular.Sample(clampSampler, texCoord).a;
    
    return float4(color, 1.0);
}