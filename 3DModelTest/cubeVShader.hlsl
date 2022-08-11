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
            // back face
            -1.0f, -1.0f, -1.0f, 
             1.0f, -1.0f, -1.0f, 
             1.0f, 1.0f, -1.0f, 
             1.0f, 1.0f, -1.0f, 
            -1.0f, 1.0f, -1.0f, 
            -1.0f, -1.0f, -1.0f, 
            // front face
            -1.0f, -1.0f, 1.0f, 
             1.0f, 1.0f, 1.0f, 
             1.0f, -1.0f, 1.0f, 
             1.0f, 1.0f, 1.0f, 
            -1.0f, -1.0f, 1.0f, 
            -1.0f, 1.0f, 1.0f, 
            // left face
            -1.0f, 1.0f, 1.0f, 
            -1.0f, -1.0f, -1.0f, 
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, 
            -1.0f, 1.0f, 1.0f, 
            -1.0f, -1.0f, 1.0f, 
            // right face
             1.0f, 1.0f, 1.0f, 
             1.0f, 1.0f, -1.0f, 
             1.0f, -1.0f, -1.0f, 
             1.0f, -1.0f, -1.0f, 
             1.0f, -1.0f, 1.0f, 
             1.0f, 1.0f, 1.0f, 
            // bottom face
            -1.0f, -1.0f, -1.0f, 
             1.0f, -1.0f, 1.0f, 
             1.0f, -1.0f, -1.0f, 
             1.0f, -1.0f, 1.0f, 
            -1.0f, -1.0f, -1.0f, 
            -1.0f, -1.0f, 1.0f, 
            // top face
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
    output.position = mul(mul(float4(position, 1.0), view), proj);
    return output;
}