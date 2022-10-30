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
    matrix normalMatrix;
}

cbuffer VoxelParam : register(b2)
{
    uint voxelGridRes;
    float voxelGridLength;
    uint2 v3;
}

Texture3D<float4> voxelTextureColor : register(t0);

[maxvertexcount(36)]
void main(
	point GSInput input[1],
	inout TriangleStream<GSOutput> output
)
{
    int3 pos = int3(input[0].vertexID / (voxelGridRes * voxelGridRes), (input[0].vertexID / voxelGridRes) % voxelGridRes, input[0].vertexID % voxelGridRes);
    
    GSOutput element;
    element.color = voxelTextureColor[pos];
    
    if (element.color.a < 0.1)
    {
        return;
    }
    
    pos -= voxelGridRes / 2;
    
    const float voxelSize = voxelGridLength / float(voxelGridRes);
    
    [unroll]
    for (uint i = 0; i < 36; i++)
    {
        element.pos = mul(mul(float4(vertices[i] * voxelSize + float3(pos) * voxelSize, 1.0), view), proj);
        output.Append(element);
    }
}