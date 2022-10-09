struct PixelInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET
{
    return float4(input.uv, 0.0, 1.0);
}