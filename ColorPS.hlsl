#include "RoughnessCB.hlsli"
#include "ColorCB.hlsli"
#include "GPassOut.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL)
{
    const float3 realViewNormal = normalize(viewNormal);
    return ConstructGPassPSOut(ColorCB.color, realViewNormal, RoughnessCB.roughness);
}