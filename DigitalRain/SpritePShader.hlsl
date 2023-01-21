struct PixelInput
{
    float2 texCoord : TEXCOORD;
    float4 color : COLOR;
};

Texture2D tex : register(t0);
SamplerState samplerState : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
    float4 color = tex.Sample(samplerState, input.texCoord);
    if(color.a<0.5)
    {
        discard;
    }
    return color * input.color;
}