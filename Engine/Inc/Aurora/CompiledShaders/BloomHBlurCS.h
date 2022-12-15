#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer BloomParam
// {
//
//   float exposure;                    // Offset:    0 Size:     4 [unused]
//   float gamma;                       // Offset:    4 Size:     4 [unused]
//   float threshold;                   // Offset:    8 Size:     4 [unused]
//   float intensity;                   // Offset:   12 Size:     4
//
// }
//
// Resource bind info for blurParams
// {
//
//   struct BlurParams
//   {
//       
//       float weight[8];               // Offset:    0
//       float offset[8];               // Offset:   32
//       float2 texelSize;              // Offset:   64
//       uint iteration;                // Offset:   72
//       float sigma;                   // Offset:   76
//
//   } $Element;                        // Offset:    0 Size:    80
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// linearSampler                     sampler      NA          NA             s0      1 
// sourceTexture                     texture  float4          2d             t0      1 
// blurParams                        texture  struct         r/o             t1      1 
// destTexture                           UAV  float4          2d             u0      1 
// BloomParam                        cbuffer      NA          NA            cb1      1 
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
dcl_constantbuffer CB1[1], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_resource_structured t1, 80
dcl_uav_typed_texture2d (float,float,float,float) u0
dcl_input vThreadID.xy
dcl_temps 9
dcl_thread_group 60, 16, 1
ld_structured_indexable(structured_buffer, stride=80)(mixed,mixed,mixed,mixed) r0.xyzw, l(0), l(0), t1.xyzw
ld_structured_indexable(structured_buffer, stride=80)(mixed,mixed,mixed,mixed) r1.xyzw, l(0), l(16), t1.xyzw
ld_structured_indexable(structured_buffer, stride=80)(mixed,mixed,mixed,mixed) r2.xyz, l(0), l(36), t1.xyzx
ld_structured_indexable(structured_buffer, stride=80)(mixed,mixed,mixed,mixed) r3.xyzw, l(0), l(48), t1.xyzw
ld_structured_indexable(structured_buffer, stride=80)(mixed,mixed,mixed,mixed) r4.xyz, l(0), l(64), t1.xyzx
utof r5.xy, vThreadID.xyxx
add r5.xy, r5.xyxx, l(0.500000, 0.500000, 0.000000, 0.000000)
mul r5.zw, r4.xxxy, r5.xxxy
sample_l_indexable(texture2d)(float,float,float,float) r6.xyz, r5.zwzz, t0.xyzw, s0, l(0.000000)
mul r6.xyz, r0.xxxx, r6.xyzx
ult r0.x, l(1), r4.z
if_nz r0.x
  mul r7.x, r2.x, r4.x
  mov r7.y, l(0)
  mad r2.xw, r5.xxxy, r4.xxxy, r7.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r8.xyz, r2.xwxx, t0.xyzw, s0, l(0.000000)
  mad r8.xyz, r8.xyzx, r0.yyyy, r6.xyzx
  mad r2.xw, r5.xxxy, r4.xxxy, -r7.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r7.xyz, r2.xwxx, t0.xyzw, s0, l(0.000000)
  mad r6.xyz, r7.xyzx, r0.yyyy, r8.xyzx
  mov r0.x, l(2)
else 
  mov r0.x, l(1)
endif 
ult r0.y, r0.x, r4.z
if_nz r0.y
  mul r2.x, r2.y, r4.x
  mov r2.y, l(0)
  mad r5.zw, r5.xxxy, r4.xxxy, r2.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r7.xyz, r5.zwzz, t0.xyzw, s0, l(0.000000)
  mad r7.xyz, r7.xyzx, r0.zzzz, r6.xyzx
  mad r2.xy, r5.xyxx, r4.xyxx, -r2.xyxx
  sample_l_indexable(texture2d)(float,float,float,float) r2.xyw, r2.xyxx, t0.xywz, s0, l(0.000000)
  mad r6.xyz, r2.xywx, r0.zzzz, r7.xyzx
  mov r0.x, l(3)
endif 
ult r0.z, r0.x, r4.z
and r0.y, r0.z, r0.y
if_nz r0.y
  mul r2.x, r2.z, r4.x
  mov r2.y, l(0)
  mad r2.zw, r5.xxxy, r4.xxxy, r2.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r7.xyz, r2.zwzz, t0.xyzw, s0, l(0.000000)
  mad r7.xyz, r7.xyzx, r0.wwww, r6.xyzx
  mad r2.xy, r5.xyxx, r4.xyxx, -r2.xyxx
  sample_l_indexable(texture2d)(float,float,float,float) r2.xyz, r2.xyxx, t0.xyzw, s0, l(0.000000)
  mad r6.xyz, r2.xyzx, r0.wwww, r7.xyzx
  mov r0.x, l(4)
endif 
ult r0.z, r0.x, r4.z
and r0.y, r0.z, r0.y
if_nz r0.y
  mul r2.x, r3.x, r4.x
  mov r2.y, l(0)
  mad r0.zw, r5.xxxy, r4.xxxy, r2.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r7.xyz, r0.zwzz, t0.xyzw, s0, l(0.000000)
  mad r7.xyz, r7.xyzx, r1.xxxx, r6.xyzx
  mad r0.zw, r5.xxxy, r4.xxxy, -r2.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r2.xyz, r0.zwzz, t0.xyzw, s0, l(0.000000)
  mad r6.xyz, r2.xyzx, r1.xxxx, r7.xyzx
  mov r0.x, l(5)
endif 
ult r0.z, r0.x, r4.z
and r0.y, r0.z, r0.y
if_nz r0.y
  mul r2.x, r3.y, r4.x
  mov r2.y, l(0)
  mad r0.zw, r5.xxxy, r4.xxxy, r2.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r7.xyz, r0.zwzz, t0.xyzw, s0, l(0.000000)
  mad r7.xyz, r7.xyzx, r1.yyyy, r6.xyzx
  mad r0.zw, r5.xxxy, r4.xxxy, -r2.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r2.xyz, r0.zwzz, t0.xyzw, s0, l(0.000000)
  mad r6.xyz, r2.xyzx, r1.yyyy, r7.xyzx
  mov r0.x, l(6)
endif 
ult r0.z, r0.x, r4.z
and r0.y, r0.z, r0.y
if_nz r0.y
  mul r1.x, r3.z, r4.x
  mov r1.y, l(0)
  mad r0.zw, r5.xxxy, r4.xxxy, r1.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r2.xyz, r0.zwzz, t0.xyzw, s0, l(0.000000)
  mad r2.xyz, r2.xyzx, r1.zzzz, r6.xyzx
  mad r0.zw, r5.xxxy, r4.xxxy, -r1.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r3.xyz, r0.zwzz, t0.xyzw, s0, l(0.000000)
  mad r6.xyz, r3.xyzx, r1.zzzz, r2.xyzx
  mov r0.x, l(7)
endif 
ult r0.x, r0.x, r4.z
and r0.x, r0.x, r0.y
if_nz r0.x
  mul r0.x, r3.w, r4.x
  mov r0.y, l(0)
  mad r0.zw, r5.xxxy, r4.xxxy, r0.xxxy
  sample_l_indexable(texture2d)(float,float,float,float) r1.xyz, r0.zwzz, t0.xyzw, s0, l(0.000000)
  mad r1.xyz, r1.xyzx, r1.wwww, r6.xyzx
  mad r0.xy, r5.xyxx, r4.xyxx, -r0.xyxx
  sample_l_indexable(texture2d)(float,float,float,float) r0.xyz, r0.xyxx, t0.xyzw, s0, l(0.000000)
  mad r6.xyz, r0.xyzx, r1.wwww, r1.xyzx
endif 
mul r0.xyz, r6.xyzx, cb1[0].wwww
mov r0.w, l(1.000000)
store_uav_typed u0.xyzw, vThreadID.xyyy, r0.xyzw
ret 
// Approximately 104 instruction slots used
#endif

const BYTE g_BloomHBlurCSBytes[] =
{
     68,  88,  66,  67, 155, 220, 
    148,  41,  62,  60,  51, 163, 
    165,  90, 172, 250, 112,  55, 
    172,  55,   1,   0,   0,   0, 
    188,  17,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     44,   4,   0,   0,  60,   4, 
      0,   0,  76,   4,   0,   0, 
     32,  17,   0,   0,  82,  68, 
     69,  70, 240,   3,   0,   0, 
      2,   0,   0,   0,  28,   1, 
      0,   0,   5,   0,   0,   0, 
     60,   0,   0,   0,   0,   5, 
     83,  67,   0, 129,   0,   0, 
    200,   3,   0,   0,  82,  68, 
     49,  49,  60,   0,   0,   0, 
     24,   0,   0,   0,  32,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    220,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 234,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0, 248,   0, 
      0,   0,   5,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,  80,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   1,   0,   0,   4,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0,  15,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 108, 105, 
    110, 101,  97, 114,  83,  97, 
    109, 112, 108, 101, 114,   0, 
    115, 111, 117, 114,  99, 101, 
     84, 101, 120, 116, 117, 114, 
    101,   0,  98, 108, 117, 114, 
     80,  97, 114,  97, 109, 115, 
      0, 100, 101, 115, 116,  84, 
    101, 120, 116, 117, 114, 101, 
      0,  66, 108, 111, 111, 109, 
     80,  97, 114,  97, 109,   0, 
    171, 171,  15,   1,   0,   0, 
      4,   0,   0,   0,  76,   1, 
      0,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 248,   0,   0,   0, 
      1,   0,   0,   0,  60,   2, 
      0,   0,  80,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0, 236,   1,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    252,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     32,   2,   0,   0,   4,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 252,   1, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  38,   2, 
      0,   0,   8,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 252,   1,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  48,   2,   0,   0, 
     12,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    252,   1,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    101, 120, 112, 111, 115, 117, 
    114, 101,   0, 102, 108, 111, 
     97, 116,   0, 171,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    245,   1,   0,   0, 103,  97, 
    109, 109,  97,   0, 116, 104, 
    114, 101, 115, 104, 111, 108, 
    100,   0, 105, 110, 116, 101, 
    110, 115, 105, 116, 121,   0, 
    171, 171, 100,   2,   0,   0, 
      0,   0,   0,   0,  80,   0, 
      0,   0,   2,   0,   0,   0, 
    164,   3,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     36,  69, 108, 101, 109, 101, 
    110, 116,   0,  66, 108, 117, 
    114,  80,  97, 114,  97, 109, 
    115,   0, 119, 101, 105, 103, 
    104, 116,   0, 171,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    245,   1,   0,   0, 111, 102, 
    102, 115, 101, 116,   0, 171, 
      0,   0,   3,   0,   1,   0, 
      1,   0,   8,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 245,   1,   0,   0, 
    116, 101, 120, 101, 108,  83, 
    105, 122, 101,   0, 102, 108, 
    111,  97, 116,  50,   0, 171, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 218,   2, 
      0,   0, 105, 116, 101, 114, 
     97, 116, 105, 111, 110,   0, 
    100, 119, 111, 114, 100,   0, 
      0,   0,  19,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  18,   3,   0,   0, 
    115, 105, 103, 109,  97,   0, 
    171, 171,   0,   0,   3,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 245,   1, 
      0,   0, 120,   2,   0,   0, 
    128,   2,   0,   0,   0,   0, 
      0,   0, 164,   2,   0,   0, 
    172,   2,   0,   0,  32,   0, 
      0,   0, 208,   2,   0,   0, 
    228,   2,   0,   0,  64,   0, 
      0,   0,   8,   3,   0,   0, 
     24,   3,   0,   0,  72,   0, 
      0,   0,  60,   3,   0,   0, 
     68,   3,   0,   0,  76,   0, 
      0,   0,   5,   0,   0,   0, 
      1,   0,  20,   0,   0,   0, 
      5,   0, 104,   3,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 109,   2, 
      0,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
     73,  83,  71,  78,   8,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  79,  83, 
     71,  78,   8,   0,   0,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  83,  72,  69,  88, 
    204,  12,   0,   0,  80,   0, 
      5,   0,  51,   3,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
    162,   0,   0,   4,   0, 112, 
     16,   0,   1,   0,   0,   0, 
     80,   0,   0,   0, 156,  24, 
      0,   4,   0, 224,  17,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,  95,   0,   0,   2, 
     50,   0,   2,   0, 104,   0, 
      0,   2,   9,   0,   0,   0, 
    155,   0,   0,   4,  60,   0, 
      0,   0,  16,   0,   0,   0, 
      1,   0,   0,   0, 167,   0, 
      0, 139,   2, 131,   2, 128, 
    131, 153,  25,   0, 242,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   1,   0,   0,   0, 
    167,   0,   0, 139,   2, 131, 
      2, 128, 131, 153,  25,   0, 
    242,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  16,   0,   0,   0, 
     70, 126,  16,   0,   1,   0, 
      0,   0, 167,   0,   0, 139, 
      2, 131,   2, 128, 131, 153, 
     25,   0, 114,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  36,   0, 
      0,   0,  70, 114,  16,   0, 
      1,   0,   0,   0, 167,   0, 
      0, 139,   2, 131,   2, 128, 
    131, 153,  25,   0, 242,   0, 
     16,   0,   3,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     48,   0,   0,   0,  70, 126, 
     16,   0,   1,   0,   0,   0, 
    167,   0,   0, 139,   2, 131, 
      2, 128, 131, 153,  25,   0, 
    114,   0,  16,   0,   4,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  64,   0,   0,   0, 
     70, 114,  16,   0,   1,   0, 
      0,   0,  86,   0,   0,   4, 
     50,   0,  16,   0,   5,   0, 
      0,   0,  70,   0,   2,   0, 
      0,   0,   0,  10,  50,   0, 
     16,   0,   5,   0,   0,   0, 
     70,   0,  16,   0,   5,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 194,   0,  16,   0, 
      5,   0,   0,   0,   6,   4, 
     16,   0,   4,   0,   0,   0, 
      6,   4,  16,   0,   5,   0, 
      0,   0,  72,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 114,   0,  16,   0, 
      6,   0,   0,   0, 230,  10, 
     16,   0,   5,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   6,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0,  79,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   1,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,  31,   0,   4,   3, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   7,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     54,   0,   0,   5,  34,   0, 
     16,   0,   7,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    146,   0,  16,   0,   2,   0, 
      0,   0,   6,   4,  16,   0, 
      5,   0,   0,   0,   6,   4, 
     16,   0,   4,   0,   0,   0, 
      6,   4,  16,   0,   7,   0, 
      0,   0,  72,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 114,   0,  16,   0, 
      8,   0,   0,   0, 198,   0, 
     16,   0,   2,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   8,   0,   0,   0, 
     70,   2,  16,   0,   8,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   6,   0,   0,   0, 
     50,   0,   0,  10, 146,   0, 
     16,   0,   2,   0,   0,   0, 
      6,   4,  16,   0,   5,   0, 
      0,   0,   6,   4,  16,   0, 
      4,   0,   0,   0,   6,   4, 
     16, 128,  65,   0,   0,   0, 
      7,   0,   0,   0,  72,   0, 
      0, 141, 194,   0,   0, 128, 
     67,  85,  21,   0, 114,   0, 
     16,   0,   7,   0,   0,   0, 
    198,   0,  16,   0,   2,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    114,   0,  16,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      7,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   8,   0, 
      0,   0,  54,   0,   0,   5, 
     18,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      2,   0,   0,   0,  18,   0, 
      0,   1,  54,   0,   0,   5, 
     18,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      1,   0,   0,   0,  21,   0, 
      0,   1,  79,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   4,   0,   0,   0, 
     31,   0,   4,   3,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     26,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   5,  34,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 194,   0, 
     16,   0,   5,   0,   0,   0, 
      6,   4,  16,   0,   5,   0, 
      0,   0,   6,   4,  16,   0, 
      4,   0,   0,   0,   6,   4, 
     16,   0,   2,   0,   0,   0, 
     72,   0,   0, 141, 194,   0, 
      0, 128,  67,  85,  21,   0, 
    114,   0,  16,   0,   7,   0, 
      0,   0, 230,  10,  16,   0, 
      5,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      7,   0,   0,   0,  70,   2, 
     16,   0,   7,   0,   0,   0, 
    166,  10,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0,  50,   0, 
      0,  10,  50,   0,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16,   0,   5,   0,   0,   0, 
     70,   0,  16,   0,   4,   0, 
      0,   0,  70,   0,  16, 128, 
     65,   0,   0,   0,   2,   0, 
      0,   0,  72,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 178,   0,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16,   0,   2,   0,   0,   0, 
     70, 123,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   6,   0,   0,   0, 
     70,   3,  16,   0,   2,   0, 
      0,   0, 166,  10,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   7,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   3,   0, 
      0,   0,  21,   0,   0,   1, 
     79,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0,   1,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   2,   0, 
      0,   0,  42,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     54,   0,   0,   5,  34,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    194,   0,  16,   0,   2,   0, 
      0,   0,   6,   4,  16,   0, 
      5,   0,   0,   0,   6,   4, 
     16,   0,   4,   0,   0,   0, 
      6,   4,  16,   0,   2,   0, 
      0,   0,  72,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 114,   0,  16,   0, 
      7,   0,   0,   0, 230,  10, 
     16,   0,   2,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   7,   0,   0,   0, 
     70,   2,  16,   0,   7,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   6,   0,   0,   0, 
     50,   0,   0,  10,  50,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   5,   0, 
      0,   0,  70,   0,  16,   0, 
      4,   0,   0,   0,  70,   0, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,  72,   0, 
      0, 141, 194,   0,   0, 128, 
     67,  85,  21,   0, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   2,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    114,   0,  16,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0, 246,  15, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   7,   0, 
      0,   0,  54,   0,   0,   5, 
     18,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      4,   0,   0,   0,  21,   0, 
      0,   1,  79,   0,   0,   7, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   4,   0,   0,   0, 
      1,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  31,   0, 
      4,   3,  26,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      2,   0,   0,   0,  10,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 194,   0,  16,   0, 
      0,   0,   0,   0,   6,   4, 
     16,   0,   5,   0,   0,   0, 
      6,   4,  16,   0,   4,   0, 
      0,   0,   6,   4,  16,   0, 
      2,   0,   0,   0,  72,   0, 
      0, 141, 194,   0,   0, 128, 
     67,  85,  21,   0, 114,   0, 
     16,   0,   7,   0,   0,   0, 
    230,  10,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    114,   0,  16,   0,   7,   0, 
      0,   0,  70,   2,  16,   0, 
      7,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   6,   0, 
      0,   0,  50,   0,   0,  10, 
    194,   0,  16,   0,   0,   0, 
      0,   0,   6,   4,  16,   0, 
      5,   0,   0,   0,   6,   4, 
     16,   0,   4,   0,   0,   0, 
      6,   4,  16, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
     72,   0,   0, 141, 194,   0, 
      0, 128,  67,  85,  21,   0, 
    114,   0,  16,   0,   2,   0, 
      0,   0, 230,  10,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
      6,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      7,   0,   0,   0,  54,   0, 
      0,   5,  18,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   5,   0,   0,   0, 
     21,   0,   0,   1,  79,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,   1,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     31,   0,   4,   3,  26,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   5,  34,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 194,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   5,   0, 
      0,   0,   6,   4,  16,   0, 
      4,   0,   0,   0,   6,   4, 
     16,   0,   2,   0,   0,   0, 
     72,   0,   0, 141, 194,   0, 
      0, 128,  67,  85,  21,   0, 
    114,   0,  16,   0,   7,   0, 
      0,   0, 230,  10,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      7,   0,   0,   0,  70,   2, 
     16,   0,   7,   0,   0,   0, 
     86,   5,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0,  50,   0, 
      0,  10, 194,   0,  16,   0, 
      0,   0,   0,   0,   6,   4, 
     16,   0,   5,   0,   0,   0, 
      6,   4,  16,   0,   4,   0, 
      0,   0,   6,   4,  16, 128, 
     65,   0,   0,   0,   2,   0, 
      0,   0,  72,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 114,   0,  16,   0, 
      2,   0,   0,   0, 230,  10, 
     16,   0,   0,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   6,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  86,   5,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   7,   0,   0,   0, 
     54,   0,   0,   5,  18,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   6,   0, 
      0,   0,  21,   0,   0,   1, 
     79,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0,   1,   0, 
      0,   7,  34,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     54,   0,   0,   5,  34,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    194,   0,  16,   0,   0,   0, 
      0,   0,   6,   4,  16,   0, 
      5,   0,   0,   0,   6,   4, 
     16,   0,   4,   0,   0,   0, 
      6,   4,  16,   0,   1,   0, 
      0,   0,  72,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 114,   0,  16,   0, 
      2,   0,   0,   0, 230,  10, 
     16,   0,   0,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0, 166,  10,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   6,   0,   0,   0, 
     50,   0,   0,  10, 194,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   4,  16,   0,   5,   0, 
      0,   0,   6,   4,  16,   0, 
      4,   0,   0,   0,   6,   4, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,  72,   0, 
      0, 141, 194,   0,   0, 128, 
     67,  85,  21,   0, 114,   0, 
     16,   0,   3,   0,   0,   0, 
    230,  10,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    114,   0,  16,   0,   6,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0, 166,  10, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
     18,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      7,   0,   0,   0,  21,   0, 
      0,   1,  79,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   4,   0,   0,   0, 
      1,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  31,   0, 
      4,   3,  10,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   4,   0, 
      0,   0,  54,   0,   0,   5, 
     34,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 194,   0,  16,   0, 
      0,   0,   0,   0,   6,   4, 
     16,   0,   5,   0,   0,   0, 
      6,   4,  16,   0,   4,   0, 
      0,   0,   6,   4,  16,   0, 
      0,   0,   0,   0,  72,   0, 
      0, 141, 194,   0,   0, 128, 
     67,  85,  21,   0, 114,   0, 
     16,   0,   1,   0,   0,   0, 
    230,  10,  16,   0,   0,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   6,   0, 
      0,   0,  50,   0,   0,  10, 
     50,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      5,   0,   0,   0,  70,   0, 
     16,   0,   4,   0,   0,   0, 
     70,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     72,   0,   0, 141, 194,   0, 
      0, 128,  67,  85,  21,   0, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,   0,  16,   0, 
      0,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 114,   0,  16,   0, 
      6,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
    246,  15,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  21,   0, 
      0,   1,  56,   0,   0,   8, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      6,   0,   0,   0, 246, 143, 
     32,   0,   1,   0,   0,   0, 
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
    104,   0,   0,   0,   9,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  39,   0, 
      0,   0,   0,   0,   0,   0, 
     12,   0,   0,   0,   2,   0, 
      0,   0,   7,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     20,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
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
