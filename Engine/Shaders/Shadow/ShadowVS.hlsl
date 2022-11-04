struct VertexInput
{
    float3 pos : POSITION;
};

cbuffer Light : register(b2)
{
    matrix lightVewProj;
};

float4 main(VertexInput input) : SV_POSITION
{
    return mul(float4(input.pos, 1.0), lightVewProj);
}