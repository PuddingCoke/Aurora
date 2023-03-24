#define M_PI 3.1415926535897932384626433832795

RWTexture2D<float2> displacementY : register(u0);
RWTexture2D<float2> displacementX : register(u1);
RWTexture2D<float2> displacementZ : register(u2);

Texture2D<float2> tildeh0k : register(t0);
Texture2D<float2> tildeh0mkconj : register(t1);

cbuffer DeltaTime : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

cbuffer OceanParam : register(b1)
{
    uint mapResolution;
    float mapLength;
    float2 wind;
    float amplitude;
    float gravity;
    float2 v1;
};

float dispersion(float2 k)
{
    float w_0 =  M_PI / 100.0;
    return floor(sqrt(gravity * length(k)) / w_0) * w_0;
}

float2 ComplexMul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float2 htilde0 = tildeh0k[DTid.xy];
    float2 htilde0mkconj = tildeh0mkconj[DTid.xy];
    
    float2 k = float2(M_PI * (2.0 * float(DTid.x) - float(mapResolution)) / float(mapLength), M_PI * (2.0 * float(DTid.y) - float(mapResolution)) / float(mapLength));
    
    float omegat = dispersion(k) * sTime;
    
    float cos_ = cos(omegat);
    float sin_ = sin(omegat);
    
    float2 c0 = float2(cos_, sin_);
    float2 c1 = float2(cos_, -sin_);
    
    float2 result = ComplexMul(htilde0, c0) + ComplexMul(htilde0mkconj, c1);
    
    displacementY[DTid.xy] = result;
    
    float len = length(k);
    
    if(len<0.0000001)
    {
        displacementX[DTid.xy] = ComplexMul(result, float2(0.0, 0.0));
        displacementZ[DTid.xy] = ComplexMul(result, float2(0.0, 0.0));
    }
    else
    {
        displacementX[DTid.xy] = ComplexMul(result, float2(0.0, -k.x / len));
        displacementZ[DTid.xy] = ComplexMul(result, float2(0.0, -k.y / len));
    }
}