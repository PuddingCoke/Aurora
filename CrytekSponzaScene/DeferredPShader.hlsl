struct PixelInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 svPosition : SV_Position;
};

struct PixelOutput
{
    float4 positionDepth : SV_TARGET0;
    float4 normalSpecular : SV_TARGET1;
    float4 albedo : SV_TARGET2;
};

Texture2D tDiffuse : register(t0);
Texture2D tSpecular : register(t1);
Texture2D tNormal : register(t2);

SamplerState samplerState : register(s0);

PixelOutput main(PixelInput input)
{
    float4 albedo = tDiffuse.Sample(samplerState, input.uv);
    
    [branch]
    if (albedo.a < 0.5)
    {
        discard;
    }
    
    float3 N = normalize(input.normal);
    float3 B = normalize(input.bitangent);
    float3 T = normalize(input.tangent);
    float3x3 TBN = float3x3(T, B, N);
    
    float3 nm = tNormal.Sample(samplerState, input.uv).xyz * 2.0 - float3(1.0, 1.0, 1.0);
    nm = mul(normalize(nm), TBN);
    
    float specular = tSpecular.Sample(samplerState, input.uv).r;
    
    PixelOutput output;
    output.positionDepth = float4(input.pos, input.svPosition.z);
    output.normalSpecular = float4(nm * 0.5 + 0.5, specular);
    output.albedo = albedo;
    
    return output;
}