struct VertexInput
{
    float2 inPos : POSITION;
};

struct VertexOutput
{
    float2 texCoord : TEXCOORD;
    float4 position : SV_Position;
};

cbuffer Matrix2D : register(b0)
{
    matrix proj;
};

static const float2 texCoords[] =
{
    { 0.0, 1.0 },
    { 0.0, 0.0 },
    { 1.0, 0.0 },
    { 1.0, 1.0 }
};

VertexOutput main(VertexInput input, in uint vertexID : SV_VertexID)
{
    VertexOutput output;
    output.texCoord = texCoords[vertexID % 4];
    output.position = mul(float4(input.inPos, 0.0, 1.0), proj);
    return output;
}