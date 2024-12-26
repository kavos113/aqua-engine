struct Type {
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

Texture2D tex : register(t0);
SamplerState sam : register(s0);

cbuffer Camera : register(b2)
{
    matrix view;
    matrix projection;
};

cbuffer Light : register(b3)
{
    float3 lightDirection;
    float3 lightColor;
}

cbuffer WorldMatrix : register(b0)
{
    matrix world;
};

Type vsMain(
    float4 position : POSITION,
    float2 uv : TEXCOORD,
    float4 normal : NORMAL
)
{
    Type output;
    output.position = mul(projection, mul(view, mul(world, position)));
    output.uv = uv;
    output.normal = mul(world, normal);

    return output;
}

float4 psMain(Type input) : SV_TARGET
{
    float3 light = normalize(lightDirection);
    float brightness = dot(input.normal.xyz, light) * -1.0f;
    brightness = saturate(brightness);

    float3 color = lightColor * brightness;

    return tex.Sample(sam, input.uv) * float4(color, 1.0f);
}