#include "Phong.hlsli"

cbuffer LightCBuf : register(b0)
{
    const float3 diffuseColor;
    const float diffuseIntensity;
    const float3 ambient;
    const float specularIntensity;
    const float3 lightViewPos;
};

cbuffer RoughnessCBuf : register(b1)
{
    const float roughness;
};

Texture2D tex : register(t1);
SamplerState texSampler : register(s1);

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL, float2 texCoord : TEX_COORD, float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POSITION) : SV_TARGET
{
    float3 realViewNormal = normalize(viewNormal);
    
    LightVectorData lightVector = CalculateLightVectorData(lightViewPos, viewPos);
    
    float lighting = CalculateLighting(lightPerspectivePos, lightVector.directionToLight, realViewNormal);
 
    const float attenuation = Attenuate(1.0f, 0.045f, 0.0075f, lightVector.distanceToLight);
	
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, attenuation, lightVector.directionToLight, realViewNormal);
	
    const float3 specular = Speculate(diffuseColor, diffuseIntensity * specularIntensity, realViewNormal, lightVector.vectorToLight, viewPos, attenuation, roughness);
	
    float3 light = lighting * saturate(diffuse + ambient + specular);
    float3 shadow = (1.0f - lighting) * ambient;
    
    return float4(tex.Sample(texSampler, texCoord).rgb * (light + shadow), 1.f);
}