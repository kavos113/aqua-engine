cbuffer CameraMatrix : register(b0)
{
    matrix view;
	matrix projection;
};

struct Type
{
    float4 svpos : SV_POSITION;
    float2 uv : TEXCOORD;
};