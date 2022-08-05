struct VertexInput
{
    float3 position : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float2 texCoord : TEXCOORD;
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

VertexOutput main( VertexInput input )
{
    VertexOutput output;
    output.position = mul(mul(float4(input.position, 1.0), view), proj);
    output.texCoord = input.texCoord;
    return output;
}