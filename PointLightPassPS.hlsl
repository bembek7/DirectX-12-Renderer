#include "PointLightPropertiesCB.hlsli"
#include "LightPass.hlsli"

float3 main(const float2 texCoord : TEX_COORD) : SV_TARGET
{
    const float3 viewPosition = sceneViewPositionTex.Sample(texSampler, texCoord).rgb;
    const float4 normal_roughness = sceneNormal_RoughnessTex.Sample(texSampler, texCoord);
    const float3 viewNormal = normal_roughness.rgb;
    const float roughness = normal_roughness.a;
    const float3 specularColor = sceneSpecularColorTex.Sample(texSampler, texCoord).rgb;
    
    const LightVectorData lightVector = CalculateLightVectorData(PointLightPropertiesCB.lightViewPos, viewPosition);
    
    const float attenuation = Attenuate(PointLightPropertiesCB.attenuationConst, PointLightPropertiesCB.attenuationLin, 
                                        PointLightPropertiesCB.attenuationQuad, lightVector.distanceToLight);
    
    const float3 diffuse = Diffuse(PointLightPropertiesCB.diffuseColor, PointLightPropertiesCB.diffuseIntensity,
                                   attenuation, lightVector.directionToLight, viewNormal);
	
    const float3 specular = Speculate(specularColor, PointLightPropertiesCB.diffuseIntensity * PointLightPropertiesCB.specularIntensity, viewNormal,
                                      lightVector.vectorToLight, viewPosition, attenuation, roughness);
    
    return saturate(diffuse + PointLightPropertiesCB.ambient + specular);
}