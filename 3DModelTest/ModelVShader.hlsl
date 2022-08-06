struct VertexInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float4 svPosition : SV_Position;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = input.position;
    output.texCoord = input.texCoord;
    output.normal = input.normal;
    output.svPosition = mul(mul(float4(input.position, 1.0), view), proj);
    return output;
}