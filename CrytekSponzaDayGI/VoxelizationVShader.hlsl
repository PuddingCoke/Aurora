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
    float3 pos : POSITION0;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
    float4 svPosition : SV_Position;
};

cbuffer VoxelProj : register(b2)
{
    matrix proj;
}

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.pos = input.pos;
    output.uv = input.uv;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.bitangent = input.bitangent;
    output.svPosition = mul(float4(input.pos, 1.0), proj);
    return output;
}