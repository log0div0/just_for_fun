#define ROOT_SIG "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT " \
                            " | DENY_HULL_SHADER_ROOT_ACCESS " \
                            " | DENY_DOMAIN_SHADER_ROOT_ACCESS " \
                            " | DENY_GEOMETRY_SHADER_ROOT_ACCESS " \
                            " | DENY_PIXEL_SHADER_ROOT_ACCESS), " \
    "RootConstants(num32BitConstants=16, b0)"

struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct Vertex
{
    float3 Position  : POSITION;
    float3 UV        : TEXCOORD;
    float3 Normal    : NORMAL;
};

struct Output
{
	float4 Color    : COLOR;
    float4 Position : SV_Position;
};

Output main(Vertex IN)
{
    Output OUT;

    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    OUT.Color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return OUT;
}