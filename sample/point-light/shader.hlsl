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

cbuffer DirectionLight : register(b3)
{
    float3 directionLightDirection;
    float3 directionLightColor;
}

cbuffer PointLight : register(b4)
{
    float3 pointLightPosition;
    float3 pointLightColor;
    float pointLightRange;
};

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

float3 LambertDiffuse(float3 normal, float3 lightDir, float3 directionLightColor)
{
    normal = normalize(normal);
    lightDir = normalize(lightDir);
    return max(0.0f, dot(normal, lightDir) * -1.0f) * directionLightColor;
}

float3 PhongSpecular(float3 normal, float3 lightDir, float3 eyeDir, float3 directionLightColor)
{
    float3 refVec = reflect(lightDir, normal);
    refVec = normalize(refVec);
    eyeDir = normalize(eyeDir);
    float spec = max(0.0f, dot(refVec, eyeDir));
    spec = pow(spec, 5.0f);

    return spec * directionLightColor;
}

float4 psMain(Type input) : SV_TARGET
{
    float3 diffuseLight = LambertDiffuse(input.normal.xyz, normalize(directionLightDirection), directionLightColor);
    float3 specularLight = PhongSpecular(input.normal.xyz, directionLightDirection, normalize(eye - input.position.xyz), directionLightColor);

    // point light
    float3 lightDir = normalize(input.position.xyz - pointLightPosition); // input light vector
    float3 diffusePointLight = LambertDiffuse(input.normal.xyz, lightDir, pointLightColor);
    float3 specularPointLight = PhongSpecular(input.normal.xyz, lightDir, normalize(eye - input.position.xyz), pointLightColor);

    float3 lightDistance = length(input.position.xyz - pointLightPosition);
    float affect = saturate(1.0f - lightDistance / pointLightRange);
    affect = pow(affect, 2.0f);

    diffuseLight += diffusePointLight * affect;
    specularLight += specularPointLight * affect;

    float3 light = specularLight * specular.xyz + diffuseLight * diffuse.xyz + ambient.xyz;

    return tex.Sample(sam, input.uv) * float4(light, 1.0f);
}