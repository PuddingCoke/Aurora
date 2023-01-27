struct PixelInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct PixelOutput
{
    float4 position : SV_TARGET0;
    float4 normalSpecular : SV_TARGET1;
    float4 baseColor : SV_TARGET2;
};

Texture2D tDiffuse : register(t0);
Texture2D tSpecular : register(t1);
Texture2D tNormal : register(t2);

SamplerState wrapSampler : register(s0);

PixelOutput main(PixelInput input)
{
    float4 baseColor = tDiffuse.Sample(wrapSampler, input.uv);
    
    [branch]
    if (baseColor.a < 0.5)
    {
        discard;
    }
    
    float3 N = normalize(input.normal);
    float3 B = normalize(input.binormal);
    float3 T = normalize(input.tangent);
    float3x3 TBN = float3x3(T, B, N);
    
    float specular = tSpecular.Sample(wrapSampler, input.uv).r;
    
    PixelOutput output;
    output.position = float4(input.pos, 1.0);
    output.normalSpecular = float4(mul(normalize(tNormal.Sample(wrapSampler, input.uv).xyz * 2.0 - 1.0), TBN), specular);
    output.baseColor = baseColor;
    
    return output;
}