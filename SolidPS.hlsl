struct Color
{
    float4 color;
};

ConstantBuffer<Color> ColorCBuf : register(b1);

float4 main() : SV_TARGET
{
    return ColorCBuf.color;
}