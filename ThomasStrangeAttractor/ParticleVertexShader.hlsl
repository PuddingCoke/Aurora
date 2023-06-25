struct VertexInput
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VertexOuput
{
    float4 color : COLOR;
    float4 position : SV_Position;
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

VertexOuput main(VertexInput input)
{
    VertexOuput output;
    output.color = input.color;
    output.position = input.position;
    return output;
}