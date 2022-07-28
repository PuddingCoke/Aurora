Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[6] = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 };

//static const float weight[6] = { 0.120891600122, 0.115466517369, 0.100608781811, 0.0799715652254, 0.0579902771337, 0.0383614182091 };

static const float offsets[4] = { 0.0, 1.465619079056, 3.420335624271, 5.0 };

static const float weight[4] = { 0.120891600122, 0.21607529918, 0.1379618423591, 0.0383614182091 };

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    float textureWidth;
    float textureHeight;
    tTexture.GetDimensions(textureWidth, textureHeight);
    const float2 tex_offset = 1.0 / float2(textureWidth, textureHeight);
    float3 result = tTexture.Sample(samplerState, texCoord).rgb * weight[0];
    
    [unroll]
    for (int i = 1; i < 4; ++i)
    {
        result += tTexture.Sample(samplerState, texCoord + float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(tex_offset.x * offsets[i], 0.0)).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}