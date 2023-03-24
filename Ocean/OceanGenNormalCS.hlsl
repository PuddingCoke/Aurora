Texture2D<float4> displacementXYZ : register(t0);

RWTexture2D<float4> normalTexture : register(u0);

cbuffer OceanParam : register(b1)
{
    uint mapResolution;
    float mapLength;
    float2 wind;
    float amplitude;
    float gravity;
    float2 v1;
};

[numthreads(32, 32, 1)]
void main(int2 DTid : SV_DispatchThreadID)
{
    float3 right = displacementXYZ[(DTid + int2(1, 0)) & (int(mapResolution) - 1)].xyz;
    float3 left = displacementXYZ[(DTid - int2(1, 0)) & (int(mapResolution) - 1)].xyz;
    float3 top = displacementXYZ[(DTid + int2(0, 1)) & (int(mapResolution) - 1)].xyz;
    float3 bottom = displacementXYZ[(DTid - int2(0, 1)) & (int(mapResolution) - 1)].xyz;
    
    float2 gradient = float2(left.y - right.y, bottom.y - top.y);
    
    float2 dDx = (right.xz - left.xz) * 2.0;
    float2 dDy = (top.xz - bottom.xz) * 2.0;
    
    float J = (1.0 + dDx.x) * (1.0 + dDy.y) - dDx.y * dDy.x;
    
    normalTexture[DTid] = float4(normalize(float3(gradient.x, 1.0, gradient.y)), J);
}