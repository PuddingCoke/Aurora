Texture2D<float3> irradianceCoeff : register(t0);

#define SQRT_PI 1.77245385090551
#define PI 3.141592653589793

float4 main(float3 position : POSITION) : SV_TARGET
{
    const float3 N = normalize(position);
    
    const float x = N.x;
    const float y = N.y;
    const float z = N.z;
    
    float A0 = PI;
    float A1 = 2.0 * PI / 3.0;
    float A2 = PI / 4.0;
    float A4 = -PI / 24.0;
    
    float3 result = A0 * irradianceCoeff[uint2(0, 0)] * (1.0 / (2.0 * SQRT_PI))
    + A1 * irradianceCoeff[uint2(1, 0)] * ((-sqrt(3.0) * y) / (2.0 * SQRT_PI))
    + A1 * irradianceCoeff[uint2(2, 0)] * ((sqrt(3.0) * z) / (2.0 * SQRT_PI))
    + A1 * irradianceCoeff[uint2(3, 0)] * ((-sqrt(3.0) * x) / (2.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint2(4, 0)] * ((sqrt(15.0) * y * x) / (2.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint2(5, 0)] * ((-sqrt(15.0) * y * z) / (2.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint2(6, 0)] * ((sqrt(5.0) * (3.0 * z * z - 1.0)) / (4.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint2(7, 0)] * ((-sqrt(15.0) * x * z) / (2.0 * SQRT_PI))
    + A2 * irradianceCoeff[uint2(8, 0)] * ((sqrt(15.0) * (x * x - y * y)) / (4.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(16, 0)] * ((3.0 * sqrt(35.0) * y * x * (x * x - y * y)) / (4.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(17, 0)] * ((-3.0 * sqrt(70.0) * y * (3.0 * x * x - y * y) * z) / (8.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(18, 0)] * ((3.0 * sqrt(5.0) * y * x * (-1.0 + 7.0 * z * z)) / (4.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(19, 0)] * ((-3.0 * sqrt(10.0) * y * z * (-3.0 + 7.0 * z * z)) / (8.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(20, 0)] * ((3.0 * (35.0 * z * z * z * z - 30.0 * z * z + 3.0)) / (16.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(21, 0)] * ((-3.0 * sqrt(10.0) * x * z * (-3.0 + 7.0 * z * z)) / (8.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(22, 0)] * ((3.0 * sqrt(5.0) * (x * x - y * y) * (-1.0 + 7.0 * z * z)) / (8.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(23, 0)] * ((-3.0 * sqrt(70.0) * x * (x * x - 3.0 * y * y) * z) / (8.0 * SQRT_PI))
    + A4 * irradianceCoeff[uint2(24, 0)] * ((3.0 * sqrt(35.0) * (x * x * x * x - 6.0 * y * y * x * x + y * y * y * y)) / (16.0 * SQRT_PI));
    
    result = max(float3(0.0, 0.0, 0.0), result);
    
    return float4(result, 1.0);
}