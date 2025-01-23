#include "RoughnessCB.hlsli"
#include "GPassOut.hlsli"
#include "DiffTex.hlsli"
#include "TextureSampler.hlsli"
#include "NormalMap.hlsli"
#include "SpecularMap.hlsli"

GPassPSOut main(float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float3 viewPosition : VIEW_POSITION)
{
    const float4 diffCol = diffTex.Sample(texSampler, texCoord);
    clip(diffCol.a < 0.05f ? -1 : 1);
    
    if (dot(viewNormal, viewPosition) >= 0.0f)
    {
        viewNormal = -viewNormal;
        tangent = -tangent;
        bitangent = -bitangent;
    }
    
    const float3 realViewNormal = CalculateViewNormal(viewNormal, tangent, bitangent, nMap.Sample(texSampler, texCoord).xyz);
    
    return ConstructGPassPSOut(diffCol.rgb, realViewNormal, RoughnessCB.roughness, viewPosition, specularMap.Sample(texSampler, texCoord).rgb);
}
