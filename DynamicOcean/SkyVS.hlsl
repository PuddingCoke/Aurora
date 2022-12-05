struct VertexInput
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
};

struct VertexOutput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 eyeToPixelVector : TEXCOORD1;
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
}

VertexOutput main( VertexInput input )
{
    VertexOutput output;
    output.pos = mul(input.pos, viewProj);
    output.uv = input.uv;
    output.eyeToPixelVector = input.pos.xyz;
    return output;
}