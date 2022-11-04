struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

struct VertexOutput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
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
    output.pos = input.pos;
    output.uv = input.uv;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.bitangent = input.bitangent;
   output.svPosition = mul(mul(float4(input.pos, 1.0), view), proj);
    return output;
}