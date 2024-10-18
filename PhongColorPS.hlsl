#include "Phong.hlsli"

struct Color
{
    float4 color;
};

ConstantBuffer<Color> ColorCBuf : register(b1);

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL, float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POSITION) : SV_TARGET
{
    const float3 realViewNormal = normalize(viewNormal);
    
    const float3 finalLight = CalculateFinalAmountOfLight(viewPos, realViewNormal, lightPerspectivePos, DirectionalLightCB.diffuseColor);
    
    return float4(ColorCBuf.color.rgb * finalLight, 1.f);
}