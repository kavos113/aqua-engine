static float PI = 3.14159265359f;

cbuffer ViewMat : register(b0)
{
    matrix view;
};

struct VS_INPUT
{
    float3 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position: SV_POSITION;
    float3 direction : TEXCOORD;
};

VS_OUTPUT vs(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position, 1.0f);
    output.direction = mul(float4(input.position, 1.0f), view).xyz;
    return output;
}

Texture2D hdrTexture : register(t0);
SamplerState hdrSampler : register(s0);

float2 EquirectangularToUV(float3 direction)
{
    float phi = atan2(direction.z, direction.x);
    float theta = acos(direction.y);

    float u = (phi + PI) / (2 * PI);
    float v = theta / PI;

    return float2(u, v);
}

float4 ps(VS_OUTPUT input) : SV_TARGET
{
    float2 uv = EquirectangularToUV(normalize(input.direction));
    float3 color = hdrTexture.Sample(hdrSampler, uv).rgb;
    return float4(color, 1.0f);
}