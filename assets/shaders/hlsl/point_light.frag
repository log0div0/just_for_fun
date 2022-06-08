
#include "common.hlsli"

cbuffer SomeStuff: register(b1)
{
	float3 LightColor;
}

float4 main() : SV_Target
{
    return float4(LightColor, 1.0f);
}
