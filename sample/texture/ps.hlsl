#include "type.hlsli"

Texture2D tex : register(t0);

SamplerState sam : register(s0);

float4 psMain(
    Type input
) : SV_TARGET
{
    return float4(tex.Sample(sam, input.uv));
}