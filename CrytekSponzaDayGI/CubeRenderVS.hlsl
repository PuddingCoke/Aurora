struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct VertexOutput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 svPosition : SV_Position;
    uint renderTargetIndex : SV_RenderTargetArrayIndex;
};

cbuffer ProjMatrices : register(b2)
{
    matrix viewProj[6];
    float4 probeLocation;
};

VertexOutput main(VertexInput input, uint instanceID : SV_InstanceID)
{
    VertexOutput output;
    output.pos = input.pos;
    output.uv = input.uv;
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.binormal = input.binormal;
    output.svPosition = mul(float4(input.pos, 1.0), viewProj[instanceID]);
    output.renderTargetIndex = instanceID;
    return output;
}