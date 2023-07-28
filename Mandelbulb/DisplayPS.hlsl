Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float3 color = tex.Sample(samplerState, texCoord).rgb;
    
    color.rgb = pow(color.rgb, 1.0 / 1.5);
    
    return float4(color, 1.0);
}