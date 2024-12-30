struct VS_INPUT 
{
    float3 position : POSITION;
};

struct VS_OUTPUT 
{
    float4 position : SV_POSITION;
    float3 direction : TEXCOORD;
};

cbuffer CameraMat : register(b0)
{
    matrix view;
    matrix projection;
    float3 eye;
};

VS_OUTPUT vs(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0f), view);
    output.direction = input.position;
    return output;
}

TextureCube cubeTexture : register(t0);
SamplerState cubeSampler : register(s0);

float4 ps(VS_OUTPUT input) : SV_TARGET
{
    float3 color = cubeTexture.Sample(cubeSampler, normalize(input.direction)).rgb;
    return float4(color, 1.0f);
}