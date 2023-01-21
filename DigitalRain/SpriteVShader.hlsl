struct VertexInput
{
    float3 position : POSITION;
    float2 size : SIZE;
    float4 color : COLOR;
    float uvLeft : TEXCOORD0;
    float uvRight : TEXCOORD1;
    float uvBottom : TEXCOORD2;
    float uvTop : TEXCOORD3;
};

struct VertexOutput
{
    float4 position : SV_Position;
    float2 size : SIZE;
    float4 color : COLOR;
    float uvLeft : TEXCOORD0;
    float uvRight : TEXCOORD1;
    float uvBottom : TEXCOORD2;
    float uvTop : TEXCOORD3;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;
    output.position = float4(input.position, 1.0);
    output.size = input.size;
    output.color = input.color;
    output.uvLeft = input.uvLeft;
    output.uvRight = input.uvRight;
    output.uvBottom = input.uvBottom;
    output.uvTop = input.uvTop;
    return output;
}