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
    matrix mat = mul(projection, mul(view, world));

    Type output;
    output.svpos = mul(mat, position);
    output.uv = uv;
    return output;
}