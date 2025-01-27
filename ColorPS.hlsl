#include "RoughnessCB.hlsli"
#include "ColorCB.hlsli"
#include "GPassOut.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL, float3 viewPosition : VIEW_POSITION, float3 worldPosition : WORLD_POSITION)
{
    const float3 realViewNormal = normalize(viewNormal);
    return ConstructGPassPSOut(ColorCB.color, realViewNormal, RoughnessCB.roughness, viewPosition, worldPosition);
}