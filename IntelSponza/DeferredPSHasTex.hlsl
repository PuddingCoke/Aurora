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
Texture2D tRoughnessMetallic : register(t1);
Texture2D tNormal : register(t2);

SamplerState linearWrapSampler : register(s0);

PixelOutput main(PixelInput input)
{
    float3 N = normalize(input.normal);
    float3 B = normalize(input.bitangent);
    float3 T = normalize(input.tangent);
    float3x3 TBN = float3x3(T, B, N);
    
    float3 nm = tNormal.Sample(linearWrapSampler, input.uv).xyz * 2.0 - 1.0;
    nm = mul(normalize(nm), TBN);
    
    PixelOutput output;
    output.baseColor = tBaseColor.Sample(linearWrapSampler, input.uv);
    output.position = float4(input.pos, 1.0);
    output.normal = float4(nm, 1.0);
    output.roughnessMetallic = float4(tRoughnessMetallic.Sample(linearWrapSampler, input.uv).gb, 0.0, 1.0);
    
    return output;
}