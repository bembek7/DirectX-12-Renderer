#include "Phong.hlsli"
#include "NormalMap.hlsli"

Texture2D tex : register(t1);
SamplerState texSampler : register(s1);

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD, 
            float3 tangent : TANGENT, float3 bitangent : BITANGENT, float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POSITION) : SV_TARGET
{
    const float3 realViewNormal = CalculateViewNormal(viewNormal, tangent, bitangent, nMap.Sample(texSampler, texCoord).xyz);
    
    const float3 finalLight = CalculateFinalAmountOfLight(viewPos, realViewNormal, lightPerspectivePos, DirectionalLightCB.diffuseColor);
    
    const float4 texSample = tex.Sample(texSampler, texCoord);
    return float4(texSample.rgb * finalLight, 1.0f);
}