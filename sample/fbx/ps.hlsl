#include "type.hlsli"

float4 psMain(Type input) : SV_TARGET
{
	float3 light = normalize(float3(-1.0, -1.0, -1.0));
	float brightness = dot(input.normal.xyz, light);

	return float4(brightness, brightness, brightness, 1.0);
}