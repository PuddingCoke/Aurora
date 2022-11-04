cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
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
    
    float3x3 rotView = (float3x3) view;
    float4 clipPos = mul(float4(mul(position, rotView), 1.0), proj);
    
    VertexOutput output;
    output.worldPos = position;
    output.position = clipPos.xyww;
    
    return output;
}