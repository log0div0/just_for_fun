struct PixelShaderInput
{
    float2 UV        : TEXCOORD;
	float3 PosWS     : POSITION;
    float3 Normal    : NORMAL;
};

float4 main( PixelShaderInput IN ) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}