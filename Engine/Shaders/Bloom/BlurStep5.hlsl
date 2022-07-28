Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

//static const float offsets[4] = { 0.0, 1.0, 2.0, 3.0 };

//static const float weight[4] = { 0.346906330784, 0.237693723707, 0.0764599801617, 0.0115467995181 };

static const float offsets[3] = { 0.0, 1.243383984400, 3.0 };

static const float weight[3] = { 0.346906330784, 0.3141537038687, 0.0115467995181 };

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
        result += tTexture.Sample(samplerState, texCoord + float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
        result += tTexture.Sample(samplerState, texCoord - float2(0.0, tex_offset.y * offsets[i])).rgb * weight[i];
    }
    
    return float4(result, 1.0);
}