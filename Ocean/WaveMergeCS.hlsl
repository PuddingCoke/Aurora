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

[numthreads(32, 32, 1)]
void main(uint2 DTid : SV_DispatchThreadID)
{
    float2 gradient = float2(-(Dyx[DTid].x / (1.0 + Dxx[DTid].x)), -(Dyz[DTid].y / (1.0 + Dzz[DTid].x)));
    
    float jacobian = (1 + Dxx[DTid].x) * (1 + Dzz[DTid].x) - Dxz[DTid].x * Dxz[DTid].x;
 
    Dxyz[DTid] = float4(Dx[DTid].x, Dy[DTid].x, Dz[DTid].x, 1.0);
    normalJacobian[DTid] = float4(normalize(float3(gradient.x, 1.0, gradient.y)), jacobian);
}