struct PixelInput
{
    float3 pos : POSITION0;
    float3 lightSpacePos : POSITION1;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 svPosition : SV_Position;
};

struct PixelOutput
{
    float4 position : SV_TARGET0;
    float4 baseColor : SV_TARGET1;
    float4 metallicRoughness : SV_TARGET2;
    float4 normal : SV_TARGET3;
    float4 lightSpacePosition : SV_TARGET4;
};

cbuffer Material: register(b3)
{
    float4 baseColor;
    float metalness;
    float roughness;
    float2 padding;
};

//wrap
SamplerState samplerState : register(s0);

PixelOutput main(PixelInput input)
{
    PixelOutput output;
    output.position = float4(input.pos, 1.0);
    output.baseColor = baseColor;
    output.metallicRoughness = float4(metalness, roughness, 0.0, 1.0);
    output.normal = float4(input.normal, 1.0);
    output.lightSpacePosition = float4(input.lightSpacePos, 1.0);
    return output;
}