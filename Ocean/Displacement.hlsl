#define M_PI 3.1415926535897932384626433832795

RWTexture2D<float2> Dy : register(u0);
RWTexture2D<float2> Dx : register(u1);
RWTexture2D<float2> Dz : register(u2);
RWTexture2D<float2> Dyx : register(u3);
RWTexture2D<float2> Dyz : register(u4);
RWTexture2D<float2> Dxx : register(u5);
RWTexture2D<float2> Dzz : register(u6);
RWTexture2D<float2> Dxz : register(u7);

Texture2D<float4> tildeh0 : register(t0);
Texture2D<float4> waveData : register(t1);

cbuffer DeltaTime : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

float2 ComplexMul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

[numthreads(32, 32, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float4 wave = waveData[DTid];
    
    float2 htilde0 = tildeh0[DTid].xy;
    
    float2 htilde0mkconj = tildeh0[DTid].zw;
    
    float omegat = wave.w * sTime;
    
    float cos_ = cos(omegat);
    float sin_ = sin(omegat);
    
    float2 c0 = float2(cos_, sin_);
    float2 c1 = float2(cos_, -sin_);
    
    float2 h = ComplexMul(htilde0, c0) + ComplexMul(htilde0mkconj, c1);
    
    float2 ih = float2(-h.y, h.x);
    
    Dy[DTid] = h;
    Dyx[DTid] = ih * wave.x;
    Dyz[DTid] = ih * wave.z;
    Dx[DTid] = ih * wave.x * wave.y;
    Dz[DTid] = ih * wave.z * wave.y;
    Dxx[DTid] = -h * wave.x * wave.x * wave.y;
    Dzz[DTid] = -h * wave.z * wave.z * wave.y;
    Dxz[DTid] = -h * wave.x * wave.z * wave.y;
}