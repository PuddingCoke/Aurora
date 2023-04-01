struct GeometryInput
{
    float4 probePosition : SV_Position;
    uint probeIndex : PROBEINDEX;
};

struct GeometryOutput
{
    float3 dir : NORMAL;
    uint probeIndex : PROBEINDEX;
    float4 svPosition : SV_Position;
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

static const float3 vertices[36] =
{
    -1.0f, -1.0f, -1.0f, 
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f, 
    1.0f, 1.0f, -1.0f, 
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f
};

[maxvertexcount(40)]
void main(
	point GeometryInput input[1],
	inout TriangleStream<GeometryOutput> outputs
)
{
    [unroll]
    for (uint i = 0; i < 36;i++)
    {
        GeometryOutput output;
        output.dir = vertices[i];
        output.probeIndex = input[0].probeIndex;
        output.svPosition = mul(float4(input[0].probePosition.xyz + vertices[i], 1.0), viewProj);
        outputs.Append(output);
    }
}