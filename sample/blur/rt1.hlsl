Texture2D tex : register(t0);
SamplerState sam : register(s0);

cbuffer Weights : register(b0)
{
    float4 weights[2];
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

float4 pshor(PSInput input) : SV_TARGET
{
    float w, h, l;
    tex.GetDimensions(0, w, h, l);
    float dx = 3.0f / w;
    float dy = 3.0f / h;

    float4 color = tex.Sample(sam, input.texcoord) * weights[0][0];

    for (int i = 1; i < 8; i++)
    {
        color += tex.Sample(sam, input.texcoord + float2(i * dx, 0.0f)) * weights[i >> 2][i % 4];
        color += tex.Sample(sam, input.texcoord - float2(i * dx, 0.0f)) * weights[i >> 2][i % 4];
    }

    return color;
}

float4 psver(PSInput input) : SV_TARGET
{
    float w, h, l;
    tex.GetDimensions(0, w, h, l);
    float dx = 3.0f / w;
    float dy = 3.0f / h;

    float4 color = tex.Sample(sam, input.texcoord) * weights[0][0];

    for (int i = 1; i < 8; i++)
    {
        color += tex.Sample(sam, input.texcoord + float2(0.0f, i * dy)) * weights[i >> 2][i % 4];
        color += tex.Sample(sam, input.texcoord - float2(0.0f, i * dy)) * weights[i >> 2][i % 4];
    }

    return color;
}