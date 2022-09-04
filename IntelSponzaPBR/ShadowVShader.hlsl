struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

cbuffer LightMatrix : register(b2)
{
    matrix lightProj;
    matrix lightView;
}
    
float4 main(VertexInput input) : SV_POSITION
{
    return mul(mul(float4(input.pos, 1.0), lightView), lightProj);
}