#include "TextureSampler.hlsli"
#include "SceneColor.hlsli"
#include "LightMap.hlsli"

float4 main(float2 texCoord : TEX_COORD) : SV_TARGET
{
    return float4(sceneColor.Sample(texSampler, texCoord).rgb * saturate(lightMap.Sample(texSampler, texCoord).rgb), 1.0f);
}