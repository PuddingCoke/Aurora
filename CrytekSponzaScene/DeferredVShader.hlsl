struct VertexInput
{
    float4 pos : POSITION;
    float2 uv : TEXCOORD;
    float3 color : COLOR;
    float3 normal : NORMAL;
    float3 tangent : TANGENT0;
    float3 bitangent : TANGENT1;
};

struct VertexOutput
{
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 color : COLOR;
    float3 worldPos : POSITION;
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
    output.svPosition = mul(mul(input.pos, view), proj);
    output.uv = input.uv;
    output.worldPos = mul(input.pos, view);
    output.normal = mul(input.normal, (float3x3) normalMatrix);
    output.tangent = normalize(input.tangent);
    output.color = input.color;
    output.bitangent = input.bitangent;
    return output;
}