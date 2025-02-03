#include "TextureSampler.hlsli"
#include "SceneColor.hlsli"
#include "LightMap.hlsli"

float4 main(float2 texCoord : TEX_COORD) : SV_TARGET
{
    const float3 color = sceneColor.Sample(texSampler, texCoord).rgb;
    const float3 light = saturate(lightMap.Sample(texSampler, texCoord).rgb);
    return float4(color * light, 1.0f);
}