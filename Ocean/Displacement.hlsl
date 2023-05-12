#define M_PI 3.1415926535897932384626433832795

RWTexture2D<float2> Dy : register(u0);
RWTexture2D<float2> Dx : register(u1);
RWTexture2D<float2> Dz : register(u2);
RWTexture2D<float2> Dyx : register(u3);
RWTexture2D<float2> Dyz : register(u4);
RWTexture2D<float2> Dxx : register(u5);
RWTexture2D<float2> Dzz : register(u6);
RWTexture2D<float2> Dxz : register(u7);

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

float Dispersion(float2 k)
{
    float w_0 = M_PI / 100.0;
    return floor(sqrt(gravity * length(k)) / w_0) * w_0;
}

float2 ComplexMul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.y * b.x + a.x * b.y);
}

[numthreads(32, 32, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float2 htilde0 = tildeh0k[DTid];
    
    float2 htilde0mkconj = tildeh0mkconj[DTid];
    
    float2 k = float2(M_PI * (2.0 * float(DTid.x) - float(mapResolution)) / float(mapLength), M_PI * (2.0 * float(DTid.y) - float(mapResolution)) / float(mapLength));
    
    float len = length(k);
    
    float omegat = Dispersion(k) * sTime;
    
    float cos_ = cos(omegat);
    float sin_ = sin(omegat);
    
    float2 c0 = float2(cos_, sin_);
    float2 c1 = float2(cos_, -sin_);
    
    float2 h = ComplexMul(htilde0, c0) + ComplexMul(htilde0mkconj, c1);
    
    float2 ih = float2(-h.y, h.x);
    
    Dy[DTid] = h;
    
    Dyx[DTid] = ih * k.x;
    Dyz[DTid] = ih * k.y;
    
    if (len < 0.000001)
    {
        Dx[DTid] = float2(0.0, 0.0);
        Dz[DTid] = float2(0.0, 0.0);
        Dxx[DTid] = float2(0.0, 0.0);
        Dzz[DTid] = float2(0.0, 0.0);
        Dxz[DTid] = float2(0.0, 0.0);
    }
    else
    {
        Dx[DTid] = ih * k.x / len;
        Dz[DTid] = ih * k.y / len;
        Dxx[DTid] = -h * k.x * k.x / len;
        Dzz[DTid] = -h * k.y * k.y / len;
        Dxz[DTid] = -h * k.x * k.y / len;
    }
}