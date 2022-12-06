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

int ClampCoord(int coord)
{
    return clamp(coord, 0, int(mapResolution - 1));
}

[numthreads(32, 32, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    int2 coord = int2(DTid.xy);
    
    float dist = mapLength / float(mapResolution);
    
    float3 right = displacementXYZ[int2(ClampCoord(coord.x + 1), coord.y)].xyz;
    float3 left = displacementXYZ[int2(ClampCoord(coord.x - 1), coord.y)].xyz;
    float3 top = displacementXYZ[int2(coord.x, ClampCoord(coord.y + 1))].xyz;
    float3 bottom = displacementXYZ[int2(coord.x, ClampCoord(coord.y - 1))].xyz;
    
    float patchSize = 20.0;
    
    float tileSize = patchSize * 2.0 / float(mapResolution);
    float invTileSize = float(mapResolution) / patchSize;
    
    float2 gradient = float2(left.y - right.y, bottom.y - top.y);
    
    float2 dDx = (right.xz - left.xz) * invTileSize;
    float2 dDy = (top.xz - bottom.xz) * invTileSize;
    
    float J = (1.0 + dDx.x) * (1.0 + dDy.y) - dDx.y * dDy.x;
    
    normalTexture[coord] = float4(gradient.x, tileSize, gradient.y, J);
}