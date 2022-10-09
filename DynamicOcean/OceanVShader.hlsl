struct VertexInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOutput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    matrix normalMatrix;
    float4 viewPos;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = input.position;
    output.uv = input.uv;
    return output;
}