#include "Phong.hlsli"
#include "NormalMap.hlsli"

Texture2D tex : register(t1);
SamplerState texSampler : register(s1);

Texture2D specularMap : register(t3);

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD,
            float3 tangent : TANGENT, float3 bitangent : BITANGENT) : SV_TARGET
{
    const float4 texSample = tex.Sample(texSampler, texCoord);
    clip(texSample.a < 0.05f ? -1 : 1);
    
    if (dot(viewNormal, viewPos) >= 0.0f)
    {
        viewNormal = -viewNormal;
        tangent = -tangent;
        bitangent = -bitangent;
    }
    
    const float3 realViewNormal = CalculateViewNormal(viewNormal, tangent, bitangent, nMap.Sample(texSampler, texCoord).xyz);
    
    const float3 finalLight = CalculateFinalAmountOfLight(viewPos, realViewNormal, specularMap.Sample(texSampler, texCoord).rgb);

    return float4(texSample.rgb * finalLight, 1.0f);
}