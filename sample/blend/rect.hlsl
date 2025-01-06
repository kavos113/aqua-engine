struct Out 
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
};

Out vs(float4 pos : POSITION, float2 uv : TEXCOORD)
{
    Out o;
    o.pos = pos;
    o.uv = uv;
    return o;
}

Texture2D tex : register(t0);
SamplerState sam : register(s0);

float4 ps(Out i) : SV_TARGET
{
    return tex.Sample(sam, i.uv);
}