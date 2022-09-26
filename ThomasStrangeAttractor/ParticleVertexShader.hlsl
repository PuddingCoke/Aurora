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
}

VertexOuput main(VertexInput input)
{
    VertexOuput output;
    output.position = mul(mul(input.position, view), proj);
    output.color = abs(input.position) * input.color * 0.7;
    return output;
}