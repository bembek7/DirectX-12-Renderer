#include "DirectionalLightPropertiesCB.hlsli"

float4 main() : SV_TARGET
{
	//const float attenuation = 1.0f;
//    const float3 diffuse = Diffuse(lightParams.diffuseColor, lightParams.diffuseIntensity, attenuation, -lightParams.lightDirection, realViewNormal);
	
//    const float3 specular = Speculate(specularColor, lightParams.diffuseIntensity * lightParams.specularIntensity, realViewNormal,
//                                    -lightParams.lightDirection, viewPos, attenuation, RoughnessCB.roughness);
    
//    return saturate(diffuse + lightParams.ambient + specular);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}