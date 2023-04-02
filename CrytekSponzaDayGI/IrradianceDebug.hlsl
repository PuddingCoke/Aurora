Texture2DArray<float3> irradianceCoeff : register(t0);

#define A0 3.141592653589793
#define A1 2.094395102393195
#define A2 0.785398163397448

#define C0 0.2820947917738781
#define C1 0.4886025119029199
#define C2 0.4886025119029199
#define C3 0.4886025119029199
#define C4 1.0925484305920790
#define C5 1.0925484305920790
#define C6 0.3153915652525200
#define C7 1.0925484305920790
#define C8 0.5462742152960395

float3 GetIrradiance(const in float3 N, const in uint probeIndex)
{
    return max(float3(0.0, 0.0, 0.0),
    A0 * C0 * irradianceCoeff[uint3(0, 0, probeIndex)]
    - A1 * C1 * irradianceCoeff[uint3(1, 0, probeIndex)] * N.y
    + A1 * C2 * irradianceCoeff[uint3(2, 0, probeIndex)] * N.z
    - A1 * C3 * irradianceCoeff[uint3(3, 0, probeIndex)] * N.x
    + A2 * C4 * irradianceCoeff[uint3(4, 0, probeIndex)] * (N.y * N.x)
    - A2 * C5 * irradianceCoeff[uint3(5, 0, probeIndex)] * (N.y * N.z)
    + A2 * C6 * irradianceCoeff[uint3(6, 0, probeIndex)] * (3.0 * N.z * N.z - 1.0)
    - A2 * C7 * irradianceCoeff[uint3(7, 0, probeIndex)] * (N.x * N.z)
    + A2 * C8 * irradianceCoeff[uint3(8, 0, probeIndex)] * (N.x * N.x - N.y * N.y));
}

float4 main(float3 position : POSITION) : SV_TARGET
{
    const float3 N = normalize(position);
    return float4(GetIrradiance(N, 0), 1.0);
}