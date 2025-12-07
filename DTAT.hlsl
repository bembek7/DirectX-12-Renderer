#include "RoughnessCB.hlsli"
#include "GPassOut.hlsli"
#include "DiffTex.hlsli"
#include "TextureSampler.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD, float3 viewPosition : VIEW_POSITION, float4 worldPosition : WORLD_POSITION)
{
    const float4 diffCol = diffTex.Sample(texSampler, texCoord);
    clip(diffCol.a < 0.05f ? -1 : 1);
    
    const float3 realViewNormal = normalize(viewNormal);
    return ConstructGPassPSOut(diffCol.rgb, realViewNormal, RoughnessCB.roughness, viewPosition, worldPosition);
}