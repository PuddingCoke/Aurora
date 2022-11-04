struct VertexInput
{
    float2 inPos : POSITION;
    float2 inTexCoord : TEXCOORD;
    float4 inColor : COLOR;
};

struct VertexOutput
{
    float2 texCoord : TEXCOORD;
    float4 color : COLOR;
    float4 position : SV_Position;
};

cbuffer Matrix2D : register(b0)
{
    matrix proj;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.texCoord = input.inTexCoord;
    output.color = input.inColor;
    output.position = mul(float4(input.inPos, 0.0, 1.0), proj);
    return output;
}