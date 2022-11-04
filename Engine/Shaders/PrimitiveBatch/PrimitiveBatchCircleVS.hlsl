struct VertexInput
{
    float2 inPos : POSITION0;
    float2 circlePos : POSITION1;
    float circleLength : POSITION2;
    float4 circleColor : COLOR;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.color = input.circleColor;
    output.position = float4(input.circleLength * input.inPos + input.circlePos, 0.0, 1.0);
    return output;
}