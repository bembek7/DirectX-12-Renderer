#include "RoughnessCB.hlsli"
#include "GPassOut.hlsli"
#include "DiffTex.hlsli"
#include "SpecularMap.hlsli"
#include "TextureSampler.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD)
{
    const float3 realViewNormal = normalize(viewNormal);
    return ConstructGPassPSOut(diffTex.Sample(texSampler, texCoord).rgb, realViewNormal, RoughnessCB.roughness, specularMap.Sample(texSampler, texCoord).rgb);
}