cbuffer ProjMatrix : register(b2)
{
    matrix viewProj;
}

static const float3 vertices[36] =
{
    -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
};

struct VertexOutput
{
    float3 worldPos : POSITION;
    float4 position : SV_Position;
};

VertexOutput main(uint vertexID : SV_VertexID)
{
    float3 position = vertices[vertexID];
    
    VertexOutput output;
    output.worldPos = position;
    output.position = mul(float4(position, 1.0), viewProj);
    
    return output;
}