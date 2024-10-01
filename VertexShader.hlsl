struct Output
{
    float4 Color : COLOR;
    float4 Position : SV_Position;
};

Output main(float3 pos : POSITION, float4 color : COLOR)
{
    Output vertexOut;

    vertexOut.Position = float4(pos, 1.0f);
    vertexOut.Color = color;
    return vertexOut;
}