struct PixelInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
    float3 eyeToPixelVector : TEXCOORD1;
};

float3 ToneMapping(float3 color)
{
    float A = 0.22;
    float B = 0.30;
    float C = 0.1;
    float D = 0.2;
    float E = 0.01;
    float F = 0.3;
    return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

#define PI 3.141592

Texture2D skyDome : register(t0);
SamplerState samplerState : register(s0);

float4 main(PixelInput input) : SV_TARGET
{
	
    float3 V = normalize(input.eyeToPixelVector);
    float3 L = normalize(float3(1.0, 0.0, 1.0));
    float sun_disk = pow(max(0.0, (dot(V, L) - 0.99995) / 0.00005), 1.001); // cos(0.57 deg) = 0.99995: Our sun disk is ~2 times larger than it is in real life

	// Getting sky color
    float2 uv = (V.xy / (1.0 + V.z)) * (0.5 / 1.5) + 0.5;
    float3 sky_color = skyDome.Sample(samplerState, uv).rgb;
    sky_color *= 1.0;
	
	// Getting sun color
    float3 sun_color = skyDome.Sample(samplerState, uv).rgb * 1.0 * sun_disk * 20.0;

	// Combining sky clolr and sun disk
    float3 color = sky_color + sun_color;

    float ExposureBias = 1.0;
    float3 curr = ToneMapping(ExposureBias * color);

    float3 whiteScale = 1.0 / ToneMapping(3.0);
    color = curr * whiteScale;

    color = pow(color, 1 / 2.2);
    return float4(color, 1.0);
}