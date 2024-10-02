struct Output
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
};

struct Rotation
{
    matrix transform;
};

ConstantBuffer<Rotation> rot : register(b0);

Output main(float3 pos : POSITION, float4 color : COLOR)
{
    Output vertexOut;

    vertexOut.Position = mul(float4(pos, 1.0f), rot.transform);
    vertexOut.Color = color;
    return vertexOut;
}