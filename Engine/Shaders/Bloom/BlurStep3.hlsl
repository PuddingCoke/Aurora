Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[3] = { 0.0, 1.0, 2.0 };

//static const float weight[3] = { 0.474931286192, 0.233820412529, 0.0279021295185 };

static const float offsets[2] = { 0.0, 1.10660957707432 };

static const float weight[2] = { 0.474931286192, 0.2617225420475 };

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
        result += tTexture.Sample(samplerState, texCoord + float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}