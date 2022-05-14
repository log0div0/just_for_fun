
cbuffer SomeStuff: register(b0)
{
    matrix MVP;
}

struct Vertex
{
    float3 Position  : POSITION;
};

struct Output
{
    float4 Position : SV_Position;
};

Output main(Vertex IN)
{
    Output OUT;

    OUT.Position = mul(MVP, float4(IN.Position, 1.0f));

    return OUT;
}
