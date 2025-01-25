#include "DirectionalLightPropertiesCB.hlsli"
#include "Phong.hlsli"
#include "TextureSampler.hlsli"
#include "SceneNormal_Roughness.hlsli"
#include "SceneSpecularColor.hlsli"
#include "SceneViewPosition.hlsli"

float3 main(const float2 texCoord : TEX_COORD) : SV_TARGET
{
    const float3 viewPosition = sceneViewPositionTex.Sample(texSampler, texCoord).rgb;
    
    const float4 normal_roughness = sceneNormal_RoughnessTex.Sample(texSampler, texCoord);
    const float3 viewNormal = normal_roughness.rgb;
    const float roughness = normal_roughness.a;
    const float3 specularColor = sceneSpecularColorTex.Sample(texSampler, texCoord).rgb;
    
	const float attenuation = 1.0f;
    const float3 diffuse = Diffuse(DirectionalLightPropertiesCB.diffuseColor, DirectionalLightPropertiesCB.diffuseIntensity, attenuation,
                                    -DirectionalLightPropertiesCB.lightDirection, viewNormal);
	
    const float3 specular = Speculate(specularColor, DirectionalLightPropertiesCB.diffuseIntensity * DirectionalLightPropertiesCB.specularIntensity,
                                    viewNormal, -DirectionalLightPropertiesCB.lightDirection, viewPosition, attenuation, roughness);
    
    return saturate(diffuse + DirectionalLightPropertiesCB.ambient + specular);
}