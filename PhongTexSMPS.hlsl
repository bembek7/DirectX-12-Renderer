#include "Phong.hlsli"

Texture2D tex : register(t1);
SamplerState texSampler : register(s1);

Texture2D specularMap : register(t3);

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD) : SV_TARGET
{
    const float3 realViewNormal = normalize(viewNormal);
    
    const float3 finalLight = CalculateFinalAmountOfLight(viewPos, realViewNormal, specularMap.Sample(texSampler, texCoord).rgb);
    
    const float4 texSample = tex.Sample(texSampler, texCoord);

    return float4(texSample.rgb * finalLight, 1.0f);
}