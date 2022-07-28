struct PixelOutput
{
    float4 color : SV_TARGET0;
    float4 brightColor : SV_TARGET1;
};

Texture2D tTexture : register(t0);
SamplerState samplerState : register(s0);

PixelOutput main(float2 texCoord : TEXCOORD)
{
    PixelOutput output;
    const float4 color = tTexture.Sample(samplerState, texCoord);
    output.color = color;
    if (dot(color.rgb, float3(0.2126, 0.7152, 0.0722))>1.0)
    {
        output.brightColor = color;
    }
    else
    {
        output.brightColor = float4(0.0, 0.0, 0.0, 1.0);
    }
    return output;
}