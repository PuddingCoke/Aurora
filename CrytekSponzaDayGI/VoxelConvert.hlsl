Texture3D<uint> voxelTextureTempColor : register(t0);
Texture3D<uint> voxelTextureTempNormal : register(t1);

RWTexture3D<float4> voxelTextureColor : register(u0);
RWTexture3D<float4> voxelTextureNormal : register(u1);

float4 RGBA8ToFloat4(uint val)
{
    return float4(float((val & 0x000000FF)),
		float((val & 0x0000FF00) >> 8U),
		float((val & 0x00FF0000) >> 16U),
		float((val & 0xFF000000) >> 24U));
}

[numthreads(8, 8, 8)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float4 color = RGBA8ToFloat4(voxelTextureTempColor[DTid]) / 255.0;
    
    if(color.a>0.1)
    {
        voxelTextureColor[DTid] = color;
        float3 N = RGBA8ToFloat4(voxelTextureTempNormal[DTid]).xyz / 255.0 * 2.0 - 1.0;
        voxelTextureNormal[DTid] = float4(N, 1.0);
    }
}