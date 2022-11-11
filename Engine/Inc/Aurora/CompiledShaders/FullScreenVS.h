#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_VertexID              0   x           0   VERTID    uint   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// TEXCOORD                 0   xy          0     NONE   float   xy  
// SV_Position              0   xyzw        1      POS   float   xyzw
//
vs_5_0
dcl_globalFlags refactoringAllowed
dcl_immediateConstantBuffer { { 0, 0, -1.000000, 1.000000},
                              { 2.000000, 0, 3.000000, 1.000000},
                              { 0, 2.000000, -1.000000, -3.000000} }
dcl_input_sgv v0.x, vertex_id
dcl_output o0.xy
dcl_output_siv o1.xyzw, position
dcl_temps 1
mov r0.x, v0.x
mov o0.xy, icb[r0.x + 0].xyxx
mov o1.xy, icb[r0.x + 0].zwzz
mov o1.zw, l(0,0,0,1.000000)
ret 
// Approximately 5 instruction slots used
#endif

const BYTE g_FullScreenVSBytes[] =
{
     68,  88,  66,  67,  36, 123, 
    214, 233, 211, 174, 119,  41, 
    250, 234,  12, 156, 202, 210, 
    232,  26,   1,   0,   0,   0, 
    176,   2,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    160,   0,   0,   0, 212,   0, 
      0,   0,  44,   1,   0,   0, 
     20,   2,   0,   0,  82,  68, 
     69,  70, 100,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     60,   0,   0,   0,   0,   5, 
    254, 255,   0,   1,   0,   0, 
     60,   0,   0,   0,  82,  68, 
     49,  49,  60,   0,   0,   0, 
     24,   0,   0,   0,  32,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  49, 
     48,  46,  49,   0,  73,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   1,   1, 
      0,   0,  83,  86,  95,  86, 
    101, 114, 116, 101, 120,  73, 
     68,   0,  79,  83,  71,  78, 
     80,   0,   0,   0,   2,   0, 
      0,   0,   8,   0,   0,   0, 
     56,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,  12,   0,   0, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   0,   0,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0,  83,  86,  95, 
     80, 111, 115, 105, 116, 105, 
    111, 110,   0, 171, 171, 171, 
     83,  72,  69,  88, 224,   0, 
      0,   0,  80,   0,   1,   0, 
     56,   0,   0,   0, 106,   8, 
      0,   1,  53,  24,   0,   0, 
     14,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 128, 191,   0,   0, 
    128,  63,   0,   0,   0,  64, 
      0,   0,   0,   0,   0,   0, 
     64,  64,   0,   0, 128,  63, 
      0,   0,   0,   0,   0,   0, 
      0,  64,   0,   0, 128, 191, 
      0,   0,  64, 192,  96,   0, 
      0,   4,  18,  16,  16,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   0,   0, 
      0,   0, 103,   0,   0,   4, 
    242,  32,  16,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    104,   0,   0,   2,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,  16,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   6,  50,  32,  16,   0, 
      0,   0,   0,   0,  70, 144, 
    144,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   6,  50,  32,  16,   0, 
      1,   0,   0,   0, 230, 154, 
    144,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   8, 194,  32,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 128,  63, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 148,   0,   0,   0, 
      5,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0
};