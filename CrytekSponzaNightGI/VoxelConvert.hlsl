Texture3D<uint> voxelTextureUint : register(t0);
RWTexture3D<float4> voxelTextureColor : register(u0);

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
    voxelTextureColor[DTid] = RGBA8ToFloat4(voxelTextureUint[DTid]) / 255.0;
}