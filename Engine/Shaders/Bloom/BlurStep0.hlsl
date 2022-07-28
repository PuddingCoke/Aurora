Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

static const float weight[2] = { 0.72534960073, 0.138900963246 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 2; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(tex_offset.x * i, 0.0)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(tex_offset.x * i, 0.0)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}