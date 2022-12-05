Texture2D<float3> irradianceCoeff : register(t0);

#define SQRT_PI 1.77245385090551
#define PI 3.141592653589793

float4 main(float3 position : POSITION) : SV_TARGET
{
    const float3 N = normalize(position);
    const float x = N.x;
    const float y = N.y;
    const float z = N.z;
    
    const float c1 = 0.42904276540489171563379376569857;
    const float c2 = 0.51166335397324424423977581244463;
    const float c3 = 0.24770795610037568833406429782001;
    const float c4 = 0.88622692545275801364908374167057;

    float3 result = max(0.0,
            (c1 * (x * x - y * y)) * irradianceCoeff[uint2(8, 0)]
            + (c3 * (3.0 * z * z - 1)) * irradianceCoeff[uint2(6, 0)]
            + c4 * irradianceCoeff[uint2(0, 0)]
            + 2.0 * c1 * (irradianceCoeff[uint2(4, 0)] * x * y + irradianceCoeff[uint2(7, 0)] * x * z + irradianceCoeff[uint2(5, 0)] * y * z)
            + 2.0 * c2 * (irradianceCoeff[uint2(3, 0)] * x + irradianceCoeff[uint2(1, 0)] * y + irradianceCoeff[uint2(2, 0)] * z));
    
    //float3 result =
    //   A0 * irradianceCoeff[uint2(0, 0)] * (1.0 / (2.0 * SQRT_PI))
    //+A1* irradianceCoeff[uint2(1, 0)] * ((-sqrt(3.0) * y) / (2.0 * SQRT_PI));
    //+A1* irradianceCoeff[uint2(2, 0)] * ((sqrt(3.0) * z) / (2.0 * SQRT_PI));
    //+A1* irradianceCoeff[uint2(3, 0)] * ((-sqrt(3.0) * x) / (2.0 * SQRT_PI))
    //+A2* irradianceCoeff[uint2(4, 0)] * ((sqrt(15.0) * y * x) / (2.0 * SQRT_PI))
    //+A2* irradianceCoeff[uint2(5, 0)] * ((-sqrt(15.0) * y * z) / (2.0 * SQRT_PI))
    //+A2* irradianceCoeff[uint2(6, 0)] * ((sqrt(5.0) * (3.0 * z * z - 1.0)) / (4.0 * SQRT_PI))
    //+A2* irradianceCoeff[uint2(7, 0)] * ((-sqrt(15.0) * x * z) / (2.0 * SQRT_PI))
    //+A2* irradianceCoeff[uint2(8, 0)] * ((sqrt(15.0) * (x * x - y * y)) / (4.0 * SQRT_PI));
    //+ irradianceCoeff[uint2(9, 0)] * ((-sqrt(70.0) * y * (3.0 * x * x - y * y)) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(10, 0)] * ((sqrt(105.0) * y * x * z) / (2.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(11, 0)] * ((-sqrt(42.0) * y * (-1.0 + 5.0 * z * z)) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(12, 0)] * ((sqrt(7.0) * z * (5.0 * z * z - 3.0)) / (4.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(13, 0)] * ((-sqrt(42.0) * x * (-1.0 + 5.0 * z * z)) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(14, 0)] * ((sqrt(105.0) * (x * x - y * y) * z) / (4.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(15, 0)] * ((-sqrt(70.0) * x * (x * x - 3.0 * y * y)) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(16, 0)] * ((3.0 * sqrt(35.0) * y * x * (x * x - y - y)) / (4.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(17, 0)] * ((-3.0 * sqrt(70.0) * y * (3.0 * x * x - y * y) * z) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(18, 0)] * ((3.0 * sqrt(5.0) * y * x * (-1.0 + 7.0 * z * z)) / (4.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(19, 0)] * ((-3.0 * sqrt(10.0) * y * z * (-3.0 + 7.0 * z * z)) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(20, 0)] * ((3.0 * (35.0 * z * z * z * z - 30.0 * z * z + 3.0)) / (16.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(21, 0)] * ((-3.0 * sqrt(10.0) * x * z * (-3.0 + 7.0 * z * z)) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(22, 0)] * ((3.0 * sqrt(5.0) * (x * x - y * y) * (-1.0 + 7.0 * z * z)) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(23, 0)] * ((-3.0 * sqrt(70.0) * x * (x * x - 3.0 * y * y) * z) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(24, 0)] * ((3.0 * sqrt(35.0) * (x * x * x * x - 6.0 * y * y * x * x + y * y * y * y)) / (16.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(25, 0)] * ((-3.0 * sqrt(154.0) * y * (5.0 * x * x * x * x - 10.0 * y * y * x * x + y * y * y * y)) / (32.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(26, 0)] * ((3.0 * sqrt(385.0) * y * x * (x * x - y * y) * z) / (4.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(27, 0)] * ((-sqrt(770.0) * y * (3.0 * x * x - y * y) * (-1.0 + 9.0 * z * z)) / (32.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(28, 0)] * ((sqrt(1155.0) * y * x * z * (-1.0 + 3.0 * z * z)) / (4.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(29, 0)] * ((-sqrt(165.0) * y * (-14.0 * z * z + 21.0 * z * z * z * z + 1.0)) / (16.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(30, 0)] * ((sqrt(11.0) * z * (63.0 * z * z * z * z - 70.0 * z * z + 15.0)) / (16.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(31, 0)] * ((-sqrt(165.0) * x * (-14.0 * z * z + 21.0 * z * z * z * z + 1.0)) / (16.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(32, 0)] * ((sqrt(1155.0) * (x * x - y * y) * z * (-1.0 + 3.0 * z * z)) / (8.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(33, 0)] * ((-sqrt(770.0) * x * (x * x - 3.0 * y * y) * (-1.0 + 9.0 * z * z)) / (32.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(34, 0)] * ((3.0 * sqrt(385.0) * (x * x * x * x - 6.0 * y * y * x * x + y * y * y * y) * z) / (16.0 * SQRT_PI))
    //+ irradianceCoeff[uint2(35, 0)] * ((-3.0 * sqrt(154.0) * x * (x * x * x * x - 10.0 * y * y * x * x + 5.0 * y * y * y * y)) / (32.0 * SQRT_PI));
    
    //result = max(float3(0.0, 0.0, 0.0), result);
    
    return float4(result, 1.0);
}