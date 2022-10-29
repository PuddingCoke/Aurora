static const float3 vertices[36] =
{
            // back face
            -1.0f, -1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            // front face
            -1.0f, -1.0f, 1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            // left face
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            // right face
             1.0f, 1.0f, 1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 1.0f,
            // bottom face
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, 1.0f,
             1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f,
            // top face
            -1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, 1.0f,
             1.0f, 1.0f, -1.0f,
             1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, 1.0f,
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

Texture3D<float4> voxelTextureColor : register(t0);

[maxvertexcount(36)]
void main(
	point GSInput input[1],
	inout TriangleStream<GSOutput> output
)
{
    int3 pos = int3(input[0].vertexID / (256 * 256), (input[0].vertexID / 256) % 256, input[0].vertexID % 256);
    
    GSOutput element;
    element.color = voxelTextureColor[pos];
    
    if (element.color.a < 0.1)
    {
        return;
    }
    
    pos -= 128;
    
    [unroll]
    for (uint i = 0; i < 36; i++)
    {
        element.pos = mul(mul(float4(vertices[i] * 300.0 / 256.0 + float3(pos) * 600.0 / 256.0, 1.0), view), proj);
        output.Append(element);
    }
}