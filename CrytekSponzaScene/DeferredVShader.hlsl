struct VertexInput
{
    float4 inPos : POSITION;
    float2 inUV : TEXCOORD;
    float3 inColor : COLOR;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT0;
    float3 inBitangent : TANGENT1;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
}

struct VertexOutput
{
    float3 outNormal : NORMAL;
    float2 outUV : TEXCOORD;
    float3 outColor : COLOR;
    float3 outWorldPos : POSITION;
    float3 outTangent : TANGENT0;
    float3 outBitangent : TANGENT1;
    float4 position : SV_Position;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    
    output.position = mul(proj, mul(view, input.inPos));
    output.outUV = input.inUV;
    output.outUV.t = 0;
    
    //pos,normal in view space
    output.outWorldPos = mul(view, input.inPos).xyz;
    float3x3 normalMatrix = transpose(inverse((float3x3) view));
    inverse();
    return output;
}