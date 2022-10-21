float4 main(float3 tex : TEXCOORD) : SV_TARGET
{
    return float4(tex, 1.0);
}