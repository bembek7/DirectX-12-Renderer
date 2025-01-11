#include "RoughnessCB.hlsli"
#include "GPassOut.hlsli"
#include "DiffTex.hlsli"
#include "TextureSampler.hlsli"
#include "NormalMap.hlsli"
#include "SpecularMap.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD, float3 tangent : TANGENT, float3 bitangent : BITANGENT)
{
    const float3 realViewNormal = CalculateViewNormal(viewNormal, tangent, bitangent, nMap.Sample(texSampler, texCoord).xyz);
    return ConstructGPassPSOut(diffTex.Sample(texSampler, texCoord).rgb, realViewNormal, RoughnessCB.roughness, specularMap.Sample(texSampler, texCoord).rgb);
}