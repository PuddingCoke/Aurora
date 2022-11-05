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
void main(uint3 DTid : SV_DispatchThreadID)
{
    int2 coord = int2(DTid.xy);
    
    float dist = mapLength / float(mapResolution);
    
    float3 center = displacementXYZ[coord].xyz;
    float3 right = float3(dist, 0.0, 0.0) + displacementXYZ[int2(clamp(coord.x + 1, 0, int(mapResolution - 1)), coord.y)].xyz - center;
    float3 left = float3(-dist, 0.0, 0.0) + displacementXYZ[int2(clamp(coord.x - 1, 0, int(mapResolution - 1)), coord.y)].xyz - center;
    float3 top = float3(0.0, 0.0, -dist) + displacementXYZ[int2(coord.x, clamp(coord.y - 1, 0, int(mapResolution - 1)))].xyz - center;
    float3 bottom = float3(0.0, 0.0, dist) + displacementXYZ[int2(coord.x, clamp(coord.y + 1, 0, int(mapResolution - 1)))].xyz - center;
    
    float3 top_right = cross(right, top);
    float3 top_left = cross(top, left);
    float3 bottom_left = cross(left, bottom);
    float3 bottom_right = cross(bottom, right);
    
    normalTexture[coord] = float4(normalize(top_right + top_left + bottom_right + bottom_left), 1.0);
}