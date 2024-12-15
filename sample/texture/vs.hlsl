#include "type.hlsli"

Type vsMain(
    float4 position : POSITION,
    float2 uv : TEXCOORD
)
{
    Type output;
    output.svpos = position;
    output.uv = uv;
    return output;
}