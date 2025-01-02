struct Type {
    float4 position : SV_POSITION;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

Texture2D tex : register(t0);
SamplerState sam : register(s0);

cbuffer Camera : register(b2)
{
    matrix view;
    matrix projection;
    float3 eye;
};

cbuffer Light : register(b3)
{
    float3 lightDirection;
    float3 lightColor;
}

cbuffer WorldMatrix : register(b0)
{
    matrix world;
};

cbuffer Material : register(b1)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

Type vsMain(
    float4 position : POSITION,
    float2 uv : TEXCOORD,
    float4 normal : NORMAL
)
{
    Type output;
    output.position = mul(projection, mul(view, mul(world, position)));
    output.uv = uv;
    output.normal = mul(world, normal);

    return output;
}

float4 psMain(Type input) : SV_TARGET
{
    // lambert
    float3 lightVec = normalize(lightDirection);
    float brightness = dot(input.normal.xyz, lightVec) * -1.0f;
    brightness = saturate(brightness);

    float3 specularLight = lightColor * brightness;

    // phong
    float3 refVec = reflect(lightDirection, input.normal.xyz);
    float3 toEye = normalize(eye - input.position.xyz);
    float spec = saturate(dot(refVec, toEye));
    spec = pow(spec, 5.0f);

    float3 diffuseLight = lightColor * spec;

    float3 light = specularLight * specular.xyz + diffuseLight * diffuse.xyz + ambient.xyz;

    return tex.Sample(sam, input.uv) * float4(light, 1.0f);
}