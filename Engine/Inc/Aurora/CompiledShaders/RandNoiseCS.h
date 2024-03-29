#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer DeltaTime
// {
//
//   float deltaTime;                   // Offset:    0 Size:     4 [unused]
//   float sumTime;                     // Offset:    4 Size:     4 [unused]
//   uint uintSeed;                     // Offset:    8 Size:     4
//   float floatSeed;                   // Offset:   12 Size:     4 [unused]
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// noiseTexture                          UAV  float4          2d             u0      1 
// DeltaTime                         cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Input
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// no Output
cs_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[1], immediateIndexed
dcl_uav_typed_texture2d (float,float,float,float) u0
dcl_input vThreadID.xy
dcl_temps 2
dcl_thread_group 32, 18, 1
mov r0.xy, vThreadID.xyxx
mov r0.z, cb0[0].z
ushr r1.xyz, r0.zxyz, l(8, 8, 8, 0)
xor r0.xyz, r0.xyzx, r1.xyzx
imul null, r0.xyz, r0.xyzx, l(0x41c64e6d, 0x41c64e6d, 0x41c64e6d, 0)
ushr r1.xyz, r0.zxyz, l(8, 8, 8, 0)
xor r0.xyz, r0.xyzx, r1.xyzx
imul null, r0.xyz, r0.xyzx, l(0x41c64e6d, 0x41c64e6d, 0x41c64e6d, 0)
ushr r1.xyz, r0.zxyz, l(8, 8, 8, 0)
xor r0.xyz, r0.xyzx, r1.xyzx
imul null, r0.xyz, r0.xyzx, l(0x41c64e6d, 0x41c64e6d, 0x41c64e6d, 0)
utof r0.xyz, r0.xyzx
mul r0.xyz, r0.xyzx, l(0.000000, 0.000000, 0.000000, 0.000000)
mov r0.w, l(1.000000)
store_uav_typed u0.xyzw, vThreadID.xyyy, r0.xyzw
ret 
// Approximately 16 instruction slots used
#endif

const BYTE g_RandNoiseCSBytes[] =
{
     68,  88,  66,  67,  38, 237, 
    232,  73, 230, 133,  72, 211, 
      6, 142,  26, 100, 139, 233, 
      7,  56,   1,   0,   0,   0, 
     32,   5,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     44,   2,   0,   0,  60,   2, 
      0,   0,  76,   2,   0,   0, 
    132,   4,   0,   0,  82,  68, 
     69,  70, 240,   1,   0,   0, 
      1,   0,   0,   0, 148,   0, 
      0,   0,   2,   0,   0,   0, 
     60,   0,   0,   0,   0,   5, 
     83,  67,   0, 129,   0,   0, 
    198,   1,   0,   0,  82,  68, 
     49,  49,  60,   0,   0,   0, 
     24,   0,   0,   0,  32,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    124,   0,   0,   0,   4,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 137,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 110, 111, 
    105, 115, 101,  84, 101, 120, 
    116, 117, 114, 101,   0,  68, 
    101, 108, 116,  97,  84, 105, 
    109, 101,   0, 171, 137,   0, 
      0,   0,   4,   0,   0,   0, 
    172,   0,   0,   0,  16,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  76,   1, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  92,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 128,   1,   0,   0, 
      4,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     92,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    136,   1,   0,   0,   8,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0, 152,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 188,   1, 
      0,   0,  12,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  92,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 100, 101, 108, 116, 
     97,  84, 105, 109, 101,   0, 
    102, 108, 111,  97, 116,   0, 
      0,   0,   3,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  86,   1,   0,   0, 
    115, 117, 109,  84, 105, 109, 
    101,   0, 117, 105, 110, 116, 
     83, 101, 101, 100,   0, 100, 
    119, 111, 114, 100,   0, 171, 
      0,   0,  19,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 145,   1,   0,   0, 
    102, 108, 111,  97, 116,  83, 
    101, 101, 100,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     49,   0, 171, 171,  73,  83, 
     71,  78,   8,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  79,  83,  71,  78, 
      8,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     83,  72,  69,  88,  48,   2, 
      0,   0,  80,   0,   5,   0, 
    140,   0,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
    156,  24,   0,   4,   0, 224, 
     17,   0,   0,   0,   0,   0, 
     85,  85,   0,   0,  95,   0, 
      0,   2,  50,   0,   2,   0, 
    104,   0,   0,   2,   2,   0, 
      0,   0, 155,   0,   0,   4, 
     32,   0,   0,   0,  18,   0, 
      0,   0,   1,   0,   0,   0, 
     54,   0,   0,   4,  50,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   0,   2,   0,  54,   0, 
      0,   6,  66,   0,  16,   0, 
      0,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  85,   0, 
      0,  10, 114,   0,  16,   0, 
      1,   0,   0,   0,  38,   9, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   8,   0, 
      0,   0,   8,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,  87,   0,   0,   7, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     38,   0,   0,  11,   0, 208, 
      0,   0, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0, 109,  78, 
    198,  65, 109,  78, 198,  65, 
    109,  78, 198,  65,   0,   0, 
      0,   0,  85,   0,   0,  10, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  38,   9,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   8,   0,   0,   0, 
      8,   0,   0,   0,   8,   0, 
      0,   0,   0,   0,   0,   0, 
     87,   0,   0,   7, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  38,   0, 
      0,  11,   0, 208,   0,   0, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0, 109,  78, 198,  65, 
    109,  78, 198,  65, 109,  78, 
    198,  65,   0,   0,   0,   0, 
     85,   0,   0,  10, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     38,   9,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      8,   0,   0,   0,   8,   0, 
      0,   0,   8,   0,   0,   0, 
      0,   0,   0,   0,  87,   0, 
      0,   7, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  38,   0,   0,  11, 
      0, 208,   0,   0, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
    109,  78, 198,  65, 109,  78, 
    198,  65, 109,  78, 198,  65, 
      0,   0,   0,   0,  86,   0, 
      0,   5, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,  10, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  47,   0,   0, 
    128,  47,   0,   0, 128,  47, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
    164,   0,   0,   6, 242, 224, 
     17,   0,   0,   0,   0,   0, 
     70,   5,   2,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 148,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0
};
