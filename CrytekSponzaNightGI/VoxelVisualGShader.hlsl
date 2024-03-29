static const float3 vertices[36] =
{
            // back face
    -0.5, -0.5, -0.5,
             0.5, 0.5, -0.5,
             0.5, -0.5, -0.5,
             0.5, 0.5, -0.5,
            -0.5, -0.5, -0.5,
            -0.5, 0.5, -0.5,
            // front face
            -0.5, -0.5, 0.5,
             0.5, -0.5, 0.5,
             0.5, 0.5, 0.5,
             0.5, 0.5, 0.5,
            -0.5, 0.5, 0.5,
            -0.5, -0.5, 0.5,
            // left face
            -0.5, 0.5, 0.5,
            -0.5, 0.5, -0.5,
            -0.5, -0.5, -0.5,
            -0.5, -0.5, -0.5,
            -0.5, -0.5, 0.5,
            -0.5, 0.5, 0.5,
            // right face
             0.5, 0.5, 0.5,
             0.5, -0.5, -0.5,
             0.5, 0.5, -0.5,
             0.5, -0.5, -0.5,
             0.5, 0.5, 0.5,
             0.5, -0.5, 0.5,
            // bottom face
            -0.5, -0.5, -0.5,
             0.5, -0.5, -0.5,
             0.5, -0.5, 0.5,
             0.5, -0.5, 0.5,
            -0.5, -0.5, 0.5,
            -0.5, -0.5, -0.5,
            // top face
            -0.5, 0.5, -0.5,
             0.5, 0.5, 0.5,
             0.5, 0.5, -0.5,
             0.5, 0.5, 0.5,
            -0.5, 0.5, -0.5,
            -0.5, 0.5, 0.5,
};

struct GSInput
{
    uint vertexID : VERTEXID;
    float4 pos : SV_Position;
};

struct GSOutput
{
    float4 color : COLOR;
    float4 pos : SV_POSITION;
};

cbuffer ProjMatrix : register(b0)
{
    matrix proj;
}

cbuffer ViewMatrix : register(b1)
{
    matrix view;
    float4 viewPos;
    matrix prevViewProj;
    matrix viewProj;
}

cbuffer VoxelParam : register(b2)
{
    uint voxelGridRes;
    float voxelGridLength;
    float voxelSize;
    float v0;
}

Texture3D<float4> voxelTextureColor : register(t0);

SamplerState pointSampler : register(s0);

[maxvertexcount(36)]
void main(
	point GSInput input[1],
	inout TriangleStream<GSOutput> output
)
{
    int3 pos = int3(input[0].vertexID / (voxelGridRes * voxelGridRes), (input[0].vertexID / voxelGridRes) % voxelGridRes, input[0].vertexID % voxelGridRes);
    float3 uvw = float3(pos) / voxelGridRes;
    
    GSOutput element;
    element.color = voxelTextureColor.SampleLevel(pointSampler, uvw, 0.0);
    
    if (element.color.a < 0.1)
    {
        return;
    }
    
    pos -= voxelGridRes / 2;
    
    [unroll]
    for (uint i = 0; i < 36; i++)
    {
        element.pos = mul(float4(vertices[i] * voxelSize + float3(pos) * voxelSize, 1.0), viewProj);
        output.Append(element);
    }
}