Texture2DArray<float4> irradianceCoeff : register(t0);

#define SQRT_PI 1.77245385090551
#define A0 3.141592653589793
#define A1 2.094395102393195
#define A2 0.785398163397448

float3 GetIrradiance(const in float3 N)
{
    return max(float3(0.0, 0.0, 0.0),
        A0 * irradianceCoeff[uint3(0, 0, 0)].rgb * (1.0 / (2.0 * SQRT_PI))
    + A1 * irradianceCoeff[uint3(1, 0, 0)].rgb * ((-sqrt(3.0) * N.y) / (2.0 * SQRT_PI))
    + A1 * irradianceCoeff[uint3(2, 0, 0)].rgb * ((sqrt(3.0) * N.z) / (2.0 * SQRT_PI))
    + A1 * irradianceCoeff[uint3(3, 0, 0)].rgb * ((-sqrt(3.0) * N.x) / (2.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint3(4, 0, 0)].rgb * ((sqrt(15.0) * N.y * N.x) / (2.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint3(5, 0, 0)].rgb * ((-sqrt(15.0) * N.y * N.z) / (2.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint3(6, 0, 0)].rgb * ((sqrt(5.0) * (3.0 * N.z * N.z - 1.0)) / (4.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint3(7, 0, 0)].rgb * ((-sqrt(15.0) * N.x * N.z) / (2.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint3(8, 0, 0)].rgb * ((sqrt(15.0) * (N.x * N.x - N.y * N.y)) / (4.0 * SQRT_PI)));
}

float4 main(float3 position : POSITION) : SV_TARGET
{
    const float3 N = normalize(position);
    return float4(GetIrradiance(N), 1.0);
}