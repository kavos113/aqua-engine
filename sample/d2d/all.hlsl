Texture2D modelTex : register(t0);
Texture2D cubemapTex : register(t1);
SamplerState sam : register(s0);

cbuffer Weight : register(b0)
{
    float weight;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
};

PSInput vs(float4 position : POSITION, float2 texcoord : TEXCOORD0)
{
    PSInput result;
    result.position = position;
    result.texcoord = texcoord;
    return result;
}

float4 ps(PSInput input) : SV_TARGET
{
    float4 modelColor = modelTex.Sample(sam, input.texcoord);
    float4 cubemapColor = cubemapTex.Sample(sam, input.texcoord);

    return modelColor * weight + cubemapColor * (1.0f - weight);
}