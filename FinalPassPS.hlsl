#include "TextureSampler.hlsli"
#include "SceneColor.hlsli"
#include "LightMap.hlsli"
#include "ShadowMap.hlsli"

float4 main(float2 texCoord : TEX_COORD) : SV_TARGET
{
    const float3 color = sceneColor.Sample(texSampler, texCoord).rgb;
    const float3 light = saturate(lightMap.Sample(texSampler, texCoord).rgb);
    const float lighting = shadowMap.Sample(texSampler, texCoord).r; // / number of lights
    return float4(color * light * lighting, 1.0f);
}