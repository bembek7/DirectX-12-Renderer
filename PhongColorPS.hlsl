#include "Phong.hlsli"

cbuffer colorCBuf : register(b2)
{
    const float4 color;
};

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL, float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POSITION) : SV_TARGET
{
    const float3 realViewNormal = normalize(viewNormal);
    
    const float3 finalLight = CalulateFinalAmountOfLight(viewPos, realViewNormal, lightPerspectivePos, diffuseColor);
    
    return float4(color.rgb * finalLight, 1.f);
}