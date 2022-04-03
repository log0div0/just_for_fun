
cbuffer Trololo: register(b8)
{
	float3 ObjectColor;
	float3 LightColor;
	float3 LightPos;
	float3 CameraPos;
}

struct PixelShaderInput
{
    float2 UV        : TEXCOORD;
	float3 PosWS     : POSITION;
    float3 Normal    : NORMAL;
};

float4 main( PixelShaderInput IN ) : SV_Target
{
    return float4(ObjectColor, 1.0f);
}