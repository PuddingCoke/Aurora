Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[5] = { 0.0, 1.0, 2.0, 3.0, 4.0 };

//static const float weight[5] = { 0.209969621264, 0.182812073509, 0.120656327901, 0.0603659656484, 0.0228944924579 };

static const float offsets[3] = { 0.0, 1.397591074854, 3.274974375335 };

static const float weight[3] = { 0.209969621264, 0.30346840141, 0.0832604581063 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 3; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}