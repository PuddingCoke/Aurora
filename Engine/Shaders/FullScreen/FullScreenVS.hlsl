static const float2 positions[] =
{
    { -1.0, 1.0 },
    { 3.0, 1.0 },
    { -1.0, -3.0 }
};

static const float2 texCoords[] =
{
    { 0.0, 0.0 },
    { 2.0, 0.0 },
    { 0.0, 2.0 }
};

struct VertexOutput
{
    float2 texCoord : TEXCOORD;
    float4 position : SV_Position;
};

VertexOutput main(uint vertexID : SV_VertexID)
{
    VertexOutput output;
    output.texCoord = texCoords[vertexID];
    output.position = float4(positions[vertexID], 0.0, 1.0);
    return output;
}