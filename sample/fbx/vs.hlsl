cbuffer SceneMatrix : register(b0)
{
    matrix view;
    matrix projection;
};

cbuffer WorldMatrix : register(b1)
{
    matrix world;
};

float4 vsMain(float4 position : POSITION) : SV_POSITION
{
    return mul(world, position);
}