#define ROOT_SIG \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "RootConstants(num32BitConstants=48, b7)"

cbuffer SomeStuff: register(b7)
{
    matrix MVP;
    matrix ModelMatrix;
    matrix<float, 3, 3> NormalMatrix;
}

struct Vertex
{
    float3 Pos       : POSITION;
    float2 UV        : TEXCOORD;
    float3 Normal    : NORMAL;
};

struct Output
{
    float2 UV        : TEXCOORD;
	float3 PosWS     : POSITION;
    float3 Normal    : NORMAL;
    float4 Pos       : SV_Position;
};

Output main(Vertex IN)
{
    Output OUT;

    OUT.UV = IN.UV;
    OUT.PosWS = mul(ModelMatrix, float4(IN.Pos, 1.0f)).xyz;
    OUT.Normal = mul(IN.Normal, NormalMatrix);
    OUT.Pos = mul(MVP, float4(IN.Pos, 1.0f));

    return OUT;
}