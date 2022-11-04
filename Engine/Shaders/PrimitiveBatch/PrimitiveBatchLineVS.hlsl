struct VertexInput
{
    float2 inPos : POSITION;
    float4 inColor : COLOR;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.color = input.inColor;
    output.position = float4(input.inPos, 0.0, 1.0);
    return output;
}