#include "type.hlsli"

Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 psMain(Type input) : SV_TARGET
{
	float3 light = normalize(float3(-1.0, -1.0, -1.0));
	float brightness = dot(input.normal.xyz, light);

	return float4(brightness, brightness, brightness, 1.0) * tex.Sample(sam, input.uv);
}