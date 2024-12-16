#include "type.hlsli"

cbuffer cbuff0 : register(b1)
{
    matrix world;
};

Type vsMain(
    float4 position : POSITION,
    float2 uv : TEXCOORD
)
{
    float4 pos = mul(world, position);

    Type output;
    output.svpos = mul(projection, mul(view, pos));
    output.uv = uv;
    return output;
}