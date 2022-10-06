#define M_PI 3.1415926535897932384626433832795

RWTexture2D<float2> displacementY : register(u0);
RWTexture2D<float2> displacementX : register(u1);
RWTexture2D<float2> displacementZ : register(u2);

Texture2D tildeh0k : register(t0);
Texture2D tildeh0mk : register(t1);

cbuffer DeltaTime : register(b0)
{
    float deltaTime;
    float sumTime;
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

float dispersion(float kx,float kz)
{
    float w_0 = 2.0 * M_PI / 200.0;
    return floor(sqrt(gravity * sqrt(kx * kx + kz * kz)) / w_0) * w_0;
}

float2 complexMul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

[numthreads(32, 32, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float2 htilde0 = tildeh0k[DTid.xy].xy;
    float2 htilde0mkconj = tildeh0mk[DTid.xy].xy;
    htilde0mkconj.y = -htilde0mkconj.y;
    
    float kx = M_PI * (2.0 * float(DTid.x) - float(mapResolution)) / mapLength;
    float kz = M_PI * (2.0 * float(DTid.y) - float(mapResolution)) / mapLength;
    
    float omegat = dispersion(kx, kz) * sumTime;
    
    float cos_ = cos(omegat);
    float sin_ = sin(omegat);
    
    float2 c0 = float2(cos_, sin_);
    float2 c1 = float2(cos_, -sin_);
    
    float2 result = complexMul(htilde0, c0) + complexMul(htilde0mkconj, c1);
    
    displacementY[DTid.xy] = result;
    displacementX[DTid.xy] = complexMul(result, float2(0.0, kx));
    displacementZ[DTid.xy] = complexMul(result, float2(0.0, kz));
}