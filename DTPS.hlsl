#include "RoughnessCB.hlsli"
#include "GPassOut.hlsli"
#include "DiffTex.hlsli"
#include "TextureSampler.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD)
{
    const float3 realViewNormal = normalize(viewNormal);
    return ConstructGPassPSOut(diffTex.Sample(texSampler, texCoord).rgb, realViewNormal, RoughnessCB.roughness);
}