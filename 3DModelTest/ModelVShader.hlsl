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
    
    float3 tPosition = input.position;
    tPosition.z -= 0.6;
    
    output.position = tPosition;
    output.texCoord = input.texCoord;
    output.normal = input.normal;
    output.svPosition = mul(mul(float4(tPosition, 1.0), view), proj);
    return output;
}