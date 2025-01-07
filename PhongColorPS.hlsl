#include "Phong.hlsli"

struct Color
{
    float4 color;
};

ConstantBuffer<Color> ColorCBuf : register(b1);

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL) : SV_TARGET
{
    const float3 realViewNormal = normalize(viewNormal);
    
    const float3 white = float3(1.0f, 1.0f, 1.0f);
    const float3 finalLight = CalculateFinalAmountOfLight(viewPos, realViewNormal, white);
    
    return float4(ColorCBuf.color.rgb * finalLight, 1.f);
}