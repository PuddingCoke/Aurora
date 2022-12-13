struct VertexInput
{
    float2 inPos : POSITION0;
    float inWidth : POSITION1;
    float4 inColor : COLOR;
};

struct VertexOutput
{
    float4 pos : SV_Position;
    float width : POSITION;
    float4 color : COLOR;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.pos = float4(input.inPos, 0.0, 1.0);
    output.width = input.inWidth;
    output.color = input.inColor;
    return output;
}