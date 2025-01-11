struct Color
{
    float3 color;
};

ConstantBuffer<Color> ColorCB : register(b1);