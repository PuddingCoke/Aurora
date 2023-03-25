struct VertexIO
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

VertexIO main(VertexIO input)
{
    VertexIO output;
    output.position = input.position;
    output.uv = input.uv;
    return output;
}