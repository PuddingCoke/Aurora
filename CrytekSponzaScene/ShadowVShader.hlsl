struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    matrix normalMatrix;
}

cbuffer Light : register(b2)
{
    matrix lightViewProj;
};

float4 main(VertexInput input) : SV_POSITION
{
    return mul(float4(input.pos, 1.0), lightViewProj);
}