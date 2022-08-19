Texture2D ssaoTexture : register(t0);

SamplerState wrapSampler : register(s0);
SamplerState linearSampler : register(s1);

float4 main(float2 texCoord : TEXCOORD) : SV_TARGET
{
    const int blurRange = 2;
    int n = 0;
    float2 texelSize = float2(1.0, 1.0) / float2(1920.0 , 1080.0);
    float result = 0.0;
    for (int x = -blurRange; x < blurRange; x++)
    {
        for (int y = -blurRange; y < blurRange; y++)
        {
            float2 offset = float2(float(x), float(y)) * texelSize;
            result += ssaoTexture.Sample(linearSampler, texCoord + offset).r;
            n++;
        }
    }

    result = result / float(n);
    
    return float4(result, result, result, 1.0f);
}