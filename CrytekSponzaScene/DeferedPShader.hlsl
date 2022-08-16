struct PixelInput
{
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 color : COLOR;
    float3 worldPos : POSITION;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 svPosition : SV_Position;
};

struct PixelOutput
{
    float4 position : SV_TARGET0;
    float4 normal : SV_TARGET1;
    float4 albedo : SV_TARGET2;
    float4 specular : SV_TARGET3;
};

Texture2D tColor : register(t0);
Texture2D tSpecular : register(t1);
Texture2D tNormal : register(t2);

SamplerState samplerState : register(s0);

PixelOutput main(PixelInput input)
{
    float4 color = tColor.Sample(samplerState, input.uv);
    
    float3 N = normalize(input.normal);
    float3 B = normalize(input.bitangent);
    float3 T = normalize(input.tangent);
    float3x3 TBN = float3x3(T, B, N);
    
    float3 nm = tNormal.Sample(samplerState, input.uv).xyz * 2.0 - float3(1.0, 1.0, 1.0);
    nm = mul(normalize(nm), TBN);
    
    float specular = tSpecular.Sample(samplerState, input.uv).r;
    
    PixelOutput output;
    output.position = float4(input.worldPos, input.svPosition.z);
    output.normal = float4(nm * 0.5 + 0.5, 0.0);
    output.albedo = color;
    output.specular = float4(specular, 0.0, 0.0, 1.0);
    return output;
}