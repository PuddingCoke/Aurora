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

cbuffer Material : register(b1)
{
    float4 baseColor;
    float roughness;
    float metallic;
    float2 v0;
}

PixelOutput main(PixelInput input)
{
    PixelOutput output;
    output.baseColor = baseColor;
    output.position = float4(input.pos, 1.0);
    output.normal = float4(input.normal, 1.0);
    output.roughnessMetallic = float4(roughness, metallic, 0.0, 1.0);
    return output;
}