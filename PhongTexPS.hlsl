#include "Phong.hlsli"

Texture2D tex : register(t1);
SamplerState texSampler : register(s1);

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD) : SV_TARGET
{
    const float3 realViewNormal = normalize(viewNormal);
    
    const float3 white = float3(1.0f, 1.0f, 1.0f);
    const float3 finalLight = CalculateFinalAmountOfLight(viewPos, realViewNormal, white);
    
    const float4 texSample = tex.Sample(texSampler, texCoord);

    return float4(texSample.rgb * finalLight, 1.0f);
}