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