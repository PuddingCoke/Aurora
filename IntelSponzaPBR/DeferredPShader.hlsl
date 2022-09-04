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

Texture2D tBaseColor : register(t0);
Texture2D tMetallicRoughness : register(t1);
Texture2D tNormal : register(t2);

//wrap
SamplerState samplerState : register(s0);

PixelOutput main(PixelInput input)
{
    float3 N = normalize(input.normal);
    float3 B = normalize(input.bitangent);
    float3 T = normalize(input.tangent);
    float3x3 TBN = float3x3(T, B, N);
    
    float3 nm = tNormal.Sample(samplerState, input.uv).xyz * 2.0 - float3(1.0, 1.0, 1.0);
    nm = mul(normalize(nm), TBN);
    
    PixelOutput output;
    output.position = float4(input.pos, 1.0);
    output.baseColor = tBaseColor.Sample(samplerState, input.uv);
    output.metallicRoughness = float4(tMetallicRoughness.Sample(samplerState, input.uv).bg, 0.0, 1.0);
    output.normal = float4(nm, 1.0);
    output.lightSpacePosition = float4(input.lightSpacePos, 1.0);
    return output;
}