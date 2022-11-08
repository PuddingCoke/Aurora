struct VertexInput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
};

struct VertexOutput
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 svPos : SV_POSITION;
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
    output.normal = input.normal;
    output.svPos = mul(mul(float4(input.pos, 1.0), view), proj);
	return output;
}