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
    matrix normalMatrix;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.pos = mul(float4(input.pos, 1.0), view).xyz;
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3) normalMatrix);
    output.tangent = input.tangent;
    output.bitangent = input.bitangent;
    output.svPosition = mul(mul(float4(input.pos, 1.0), view), proj);
    return output;
}