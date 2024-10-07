struct Color
{
    float4 color;
};

ConstantBuffer<Color> ColorCBuf : register(b2);

float4 main() : SV_TARGET
{
    return ColorCBuf.color;
}