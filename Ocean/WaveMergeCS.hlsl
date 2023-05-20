Texture2D<float2> Dy : register(t0);
Texture2D<float2> Dx : register(t1);
Texture2D<float2> Dz : register(t2);
Texture2D<float2> Dyx : register(t3);
Texture2D<float2> Dyz : register(t4);
Texture2D<float2> Dxx : register(t5);
Texture2D<float2> Dzz : register(t6);
Texture2D<float2> Dxz : register(t7);

RWTexture2D<float4> Dxyz : register(u0);
RWTexture2D<float4> normalJacobian : register(u1);

static const float lambda = 1.0;

cbuffer DeltaTime : register(b0)
{
    float deltaTime;
    float sTime;
    float2 v0;
}

[numthreads(32, 32, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float2 slope = float2(Dyx[DTid].x / (1.0 + lambda * Dxx[DTid].x), Dyz[DTid].x / (1.0 + lambda * Dzz[DTid].x));
    
    float jacobian = (1 + lambda * Dxx[DTid].x) * (1 + lambda * Dzz[DTid].x) - lambda * lambda * Dxz[DTid].x * Dxz[DTid].x;
 
    float temp = normalJacobian[DTid].w + deltaTime * 0.5 / max(jacobian, 0.5);
    
    jacobian = min(jacobian, temp);
    
    Dxyz[DTid] = float4(lambda * Dx[DTid].x, Dy[DTid].x, lambda * Dz[DTid].x, 1.0);
    
    normalJacobian[DTid] = float4(normalize(float3(-slope.x, 1.0, -slope.y)), jacobian);
}