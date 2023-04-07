struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct VertexOutput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 svPosition : SV_Position;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
        matrix view;
        float4 viewPos;
        matrix prevViewProj;
        matrix viewProj;
        matrix normalMatrix;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.pos = input.pos;
    output.uv = input.uv;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.binormal = input.binormal;
    output.svPosition = mul(float4(input.pos, 1.0), viewProj);
    return output;
}