#include "RoughnessCB.hlsli"
#include "ColorCB.hlsli"
#include "GPassOut.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL)
{
    const float3 realViewNormal = normalize(viewNormal);
    const float3 specularColor = float3(1.0f, 1.0f, 1.0f);
    return ConstructGPassPSOut(ColorCB.color, realViewNormal, RoughnessCB.roughness);
}