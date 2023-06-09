struct VSInput
{
    float2 position : POSITION;
    float2 velocity : VELOCITY;
};

struct VSOutput
{
    float4 position : SV_Position;
    float2 velocity : VELOCITY;
};

VSOutput main(VSInput input)
{
    VSOutput output;
    output.position = float4(input.position, 0.1, 1.0);
    output.velocity = input.velocity;
    return output;
}