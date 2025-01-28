#include "RoughnessCB.hlsli"
#include "GPassOut.hlsli"
#include "DiffTex.hlsli"
#include "SpecularMap.hlsli"
#include "TextureSampler.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD, float3 viewPosition : VIEW_POSITION, float4 worldPosition : WORLD_POSITION)
{
    const float3 realViewNormal = normalize(viewNormal);
    return ConstructGPassPSOut(diffTex.Sample(texSampler, texCoord).rgb, realViewNormal, RoughnessCB.roughness, viewPosition, worldPosition, specularMap.Sample(texSampler, texCoord).rgb);
}