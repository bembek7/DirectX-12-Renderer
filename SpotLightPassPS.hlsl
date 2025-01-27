#include "SpotLightPropertiesCB.hlsli"
#include "LightPass.hlsli"

float CalculateSpotFactor(const float3 lightDirection, const float3 spotPower, const float3 directionToLight)
{
    return pow(max(dot(-directionToLight, lightDirection), 0.0f), spotPower).x;
}

float3 main(const float2 texCoord : TEX_COORD) : SV_TARGET
{
    const float3 viewPosition = sceneViewPositionTex.Sample(texSampler, texCoord).rgb;
    const float4 normal_roughness = sceneNormal_RoughnessTex.Sample(texSampler, texCoord);
    const float3 viewNormal = normal_roughness.rgb;
    const float roughness = normal_roughness.a;
    const float3 specularColor = sceneSpecularColorTex.Sample(texSampler, texCoord).rgb;
    const float3 worldPosition = sceneWorldPositionTex.Sample(texSampler, texCoord).rgb;
   
    const float4 lightPerspectivePosition = mul(mul(float4(worldPosition, 1.0f), LightPerspectiveCB.view), LightPerspectiveCB.proj);
    
    const float lighting = CalculateLighting(lightPerspectivePosition);
    
    const LightVectorData lightVector = CalculateLightVectorData(SpotLightPropertiesCB.lightViewPos, viewPosition);
    
    float attenuation = Attenuate(SpotLightPropertiesCB.attenuationConst, SpotLightPropertiesCB.attenuationLin, 
                                   SpotLightPropertiesCB.attenuationQuad, lightVector.distanceToLight);
    
    const float spotFactor = CalculateSpotFactor(SpotLightPropertiesCB.lightDirection, SpotLightPropertiesCB.spotPower, lightVector.directionToLight);

    attenuation *= spotFactor;
    
    const float3 diffuse = Diffuse(SpotLightPropertiesCB.diffuseColor, SpotLightPropertiesCB.diffuseIntensity, attenuation, 
                                    lightVector.directionToLight, viewNormal);
	
    const float3 specular = Speculate(specularColor, SpotLightPropertiesCB.diffuseIntensity * SpotLightPropertiesCB.specularIntensity, viewNormal,
                                    lightVector.vectorToLight, viewPosition, attenuation, roughness);
    
    return saturate((diffuse + specular) * lighting + SpotLightPropertiesCB.ambient);
}