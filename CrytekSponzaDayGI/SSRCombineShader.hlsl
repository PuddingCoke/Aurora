Texture2D originTexture : register(t0);
Texture2D uvVisibilityTexture : register(t1);
Texture2D gNormalSpecular : register(t2);

SamplerState wrapSampler : register(s0);
SamplerState clampSampler : register(s1);
SamplerState pointSampler : register(s2);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 color = originTexture.Sample(clampSampler, texCoord).rgb;
    
    float4 uvVisibility = uvVisibilityTexture.Sample(clampSampler, texCoord);
    
    float specular = gNormalSpecular.Sample(clampSampler, texCoord).w;
    
    float3 reflectedColor = color + originTexture.Sample(clampSampler, uvVisibility.xy).rgb * uvVisibility.z * specular;
	
    return float4(reflectedColor, 1.0);
}