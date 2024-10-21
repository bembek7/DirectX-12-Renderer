//#include "Shadow.hlsli"

float Attenuate(uniform float attenuationConst, uniform float attenuationLin, uniform float attenuationQuad, const in float distanceToL)
{
    return 1.0f / (attenuationConst + attenuationLin * distanceToL + attenuationQuad * (distanceToL * distanceToL));
}

float3 Diffuse(
    uniform float3 diffuseColor,
    uniform float diffuseIntensity,
    const in float attenuation,
    const in float3 viewDirToL,
    const in float3 viewNormal)
{
    return diffuseColor * diffuseIntensity * attenuation * max(0.0f, dot(viewDirToL, viewNormal));
}

float3 Speculate(
    const in float3 specularColor,
    uniform float specularIntensity,
    const in float3 viewNormal,
    const in float3 viewToLight,
    const in float3 viewPos,
    const in float attenuation,
    const in float specularPower)
{
    const float3 w = viewNormal * dot(viewToLight, viewNormal);
    const float3 reflected = normalize(w * 2.0f - viewToLight);

    const float3 viewCamToPos = normalize(viewPos);

    return attenuation * specularColor * specularIntensity * pow(max(0.0f, dot(-reflected, viewCamToPos)), specularPower);
}

struct LightVectorData
{
    float3 vectorToLight;
    float3 directionToLight;
    float distanceToLight;
};

LightVectorData CalculateLightVectorData(float3 lightViewPos, float3 viewPos)
{
    LightVectorData lightVector;
    lightVector.vectorToLight = lightViewPos - viewPos;
    lightVector.distanceToLight = length(lightVector.vectorToLight);
    lightVector.directionToLight = lightVector.vectorToLight / lightVector.distanceToLight;
    
    return lightVector;
}

struct DirectionalLight
{
    float3 diffuseColor;        
    float diffuseIntensity;     
    float3 ambient;             
    float specularIntensity;    
    float3 lightDirection;      
};

struct PointLight
{
    float3 diffuseColor;
    float diffuseIntensity;
    float3 ambient;
    float specularIntensity;
    float3 lightViewPos;
    float attenuationConst;
    float attenuationLin;
    float attenuationQuad;
};

struct SpotLight
{
    float3 diffuseColor;
    float diffuseIntensity; 
    float3 ambient;
    float specularIntensity;
    float3 lightViewPos;
    float spotPower;
    float3 lightDirection;
    float attenuationConst;
    float attenuationLin; 
    float attenuationQuad; 
};

#define DIRECTIONAL_LIGHTS_NUM 1
#define POINT_LIGHTS_NUM 1
#define SPOT_LIGHTS_NUM 1

struct DirectionalLights
{
    DirectionalLight directionalLights[DIRECTIONAL_LIGHTS_NUM];
};

struct PointLights
{
    PointLight pointLights[POINT_LIGHTS_NUM];
};

struct SpotLights
{
    SpotLight spotLights[SPOT_LIGHTS_NUM];
};

struct Roughness
{
    float roughness;
};

ConstantBuffer<Roughness> RoughnessCB : register(b0);

ConstantBuffer<DirectionalLights> DirectionalLightsCB : register(b2);
ConstantBuffer<PointLights> PointLightsCB : register(b3);
ConstantBuffer<SpotLights> SpotLightsCB : register(b4);


float3 CalculateDirectionalLight(const DirectionalLight lightParams, const float3 viewPos, const float3 realViewNormal, const float3 specularColor)
{
    const float attenuation = 1.0f;
    const float3 diffuse = Diffuse(lightParams.diffuseColor, lightParams.diffuseIntensity, attenuation, -lightParams.lightDirection, realViewNormal);
	
    const float3 specular = Speculate(specularColor, lightParams.diffuseIntensity * lightParams.specularIntensity, realViewNormal,
                                    -lightParams.lightDirection, viewPos, attenuation, RoughnessCB.roughness);
    
    return saturate(diffuse + lightParams.ambient + specular);
}

float3 CalculatePointLight(const PointLight lightParams, const float3 viewPos, const float3 realViewNormal, const float3 specularColor)
{
    const LightVectorData lightVector = CalculateLightVectorData(lightParams.lightViewPos, viewPos);
    
    const float attenuation = Attenuate(lightParams.attenuationConst, lightParams.attenuationLin, lightParams.attenuationQuad, lightVector.distanceToLight);
    const float3 diffuse = Diffuse(lightParams.diffuseColor, lightParams.diffuseIntensity, attenuation, lightVector.directionToLight, realViewNormal);
	
    const float3 specular = Speculate(specularColor, lightParams.diffuseIntensity * lightParams.specularIntensity, realViewNormal,
                                    lightVector.vectorToLight, viewPos, attenuation, RoughnessCB.roughness);
    
    return saturate(diffuse + lightParams.ambient + specular);
}

float CalculateSpotFactor(const SpotLight lightParams, const float3 directionToLight)
{
    return pow(max(dot(-directionToLight, lightParams.lightDirection), 0.0f), lightParams.spotPower);
}

float3 CalculateSpotLight(const SpotLight lightParams, const float3 viewPos, const float3 realViewNormal, const float3 specularColor)
{
    const LightVectorData lightVector = CalculateLightVectorData(lightParams.lightViewPos, viewPos);
    
    float attenuation = Attenuate(lightParams.attenuationConst, lightParams.attenuationLin, lightParams.attenuationQuad, lightVector.distanceToLight);
    
    const float spotFactor = CalculateSpotFactor(lightParams, lightVector.directionToLight);

    attenuation *= spotFactor;
    
    const float3 diffuse = Diffuse(lightParams.diffuseColor, lightParams.diffuseIntensity, attenuation, lightVector.directionToLight, realViewNormal);
	
    const float3 specular = Speculate(specularColor, lightParams.diffuseIntensity * lightParams.specularIntensity, realViewNormal,
                                    lightVector.vectorToLight, viewPos, attenuation, RoughnessCB.roughness);
    
    return saturate(diffuse + lightParams.ambient + specular);
}

float3 CalculateFinalAmountOfLight(const float3 viewPos, const float3 realViewNormal, const float4 lightPerspectivePos, const float3 specularColor)
{
    const float lighting = 1.0f;
    //const float lighting = CalculateLighting(lightPerspectivePos);
    float3 light = float3(0.0f, 0.0f, 0.0f);
    
    [unroll]
    for (int d = 0; d < DIRECTIONAL_LIGHTS_NUM; ++d)
    {
        light += CalculateDirectionalLight(DirectionalLightsCB.directionalLights[d], viewPos, realViewNormal, specularColor);
    }
    [unroll]
    for (int p = 0; p < POINT_LIGHTS_NUM; ++p)
    {
        light += CalculatePointLight(PointLightsCB.pointLights[p], viewPos, realViewNormal, specularColor);
    }
    [unroll]
    for (int s = 0; s < SPOT_LIGHTS_NUM; ++s)
    {
        light += CalculateSpotLight(SpotLightsCB.spotLights[s], viewPos, realViewNormal, specularColor);
    }
    
    light = lighting * saturate(light);
    //const float3 shadow = (1.0f - lighting) * DirectionalLightCB.ambient;
    const float3 shadow = 0.0f;
    
    return light + shadow;
}


