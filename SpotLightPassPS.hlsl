#include "SpotLightPropertiesCB.hlsli"

//float CalculateSpotFactor(const SpotLight lightParams, const float3 directionToLight)
//{
//    return pow(max(dot(-directionToLight, lightParams.lightDirection), 0.0f), lightParams.spotPower);
//}

float4 main() : SV_TARGET
{
	//    const LightVectorData lightVector = CalculateLightVectorData(lightParams.lightViewPos, viewPos);
    
//    float attenuation = Attenuate(lightParams.attenuationConst, lightParams.attenuationLin, lightParams.attenuationQuad, lightVector.distanceToLight);
    
//    const float spotFactor = CalculateSpotFactor(lightParams, lightVector.directionToLight);

//    attenuation *= spotFactor;
    
//    const float3 diffuse = Diffuse(lightParams.diffuseColor, lightParams.diffuseIntensity, attenuation, lightVector.directionToLight, realViewNormal);
	
//    const float3 specular = Speculate(specularColor, lightParams.diffuseIntensity * lightParams.specularIntensity, realViewNormal,
//                                    lightVector.vectorToLight, viewPos, attenuation, RoughnessCB.roughness);
    
//    return saturate(diffuse + lightParams.ambient + specular);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}