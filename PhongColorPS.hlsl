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

cbuffer colorCBuf : register(b2)
{
    const float4 color;
};

float4 main(float3 viewPos : POSITION, float3 viewNormal : NORMAL, float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POSITION) : SV_TARGET
{
    float3 realViewNormal = normalize(viewNormal);
    
    LightVectorData lightVector = CalculateLightVectorData(lightViewPos, viewPos);
    
    float lighting = CalculateLighting(lightPerspectivePos, lightVector.directionToLight, realViewNormal);
    
    const float attenuation = Attenuate(1.0f, 0.045f, 0.0075f, lightVector.distanceToLight);
	
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, attenuation, lightVector.directionToLight, realViewNormal);
	
    const float3 specular = Speculate(diffuseColor, diffuseIntensity * specularIntensity, realViewNormal, lightVector.vectorToLight, viewPos, attenuation, roughness);
	
    return float4(saturate((diffuse * lighting + ambient) * color.rgb + specular * lighting), color.a);
}