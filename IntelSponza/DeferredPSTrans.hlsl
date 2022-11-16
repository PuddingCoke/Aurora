struct PixelInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

struct PixelOutput
{
    float4 baseColor : SV_TARGET0;
    float4 position : SV_TARGET1;
    float4 normal : SV_TARGET2;
    float4 roughnessMetallic : SV_TARGET3;
};

Texture2D tBaseColor : register(t0);

SamplerState linearWrapSampler : register(s0);

PixelOutput main(PixelInput input)
{
    PixelOutput output;
    float4 color = tBaseColor.Sample(linearWrapSampler, input.uv);
    output.baseColor = color;
    output.position = float4(input.pos, color.a);
    output.normal = float4(input.normal, color.a);
    output.roughnessMetallic = float4(1.0, 0.0, 0.0, color.a);
    return output;
}