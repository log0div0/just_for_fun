
cbuffer Trololo: register(b1)
{
	float3 ObjectColor;
	float3 LightColor;
	float3 LightPos;
	float3 CameraPos;
}

Texture2D<float4> Wood : register(t0);
Texture2D<float4> Lambda : register(t1);

SamplerState Sampler : register(s0);

struct PixelShaderInput
{
    float2 UV        : TEXCOORD;
	float3 PosWS     : POSITION;
    float3 Normal    : NORMAL;
};

float4 main( PixelShaderInput IN ) : SV_Target
{
	float3 ToLight = normalize(LightPos - IN.PosWS);
	float3 FromLight = -ToLight;
	float3 ToCamera = normalize(CameraPos - IN.PosWS);
	float3 ReflectDir = reflect(FromLight, IN.Normal);

	float Ambient = 0.1;
	float Diffuse = max(0.0, dot(ToLight, IN.Normal));
	float SpecularStrength = 0.5;
	float Specular = pow(max(0.0, dot(ToCamera, ReflectDir)), 64) * SpecularStrength;

	float4 SurfaceColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	SurfaceColor = lerp(Lambda.Sample(Sampler, IN.UV), Wood.Sample(Sampler, IN.UV), 1 - Lambda.Sample(Sampler, IN.UV).w);
    return float4((Ambient + Diffuse + Specular) * LightColor, 1.0f) * SurfaceColor;
}
