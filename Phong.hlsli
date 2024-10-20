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

struct PointLight
{
    float3 diffuseColor;        //all
    float diffuseIntensity;     //all
    float3 ambient;             //all
    float specularIntensity;    //all
    float3 lightViewPos;        //point/spot
    float attenuationConst;     //point/spot
    float attenuationLin;       //point/spot
    float attenuationQuad;      //point/spot
};

struct DirectionalLight
{
    float3 diffuseColor;        
    float diffuseIntensity;     
    float3 ambient;             
    float specularIntensity;    
    float3 lightDirection;      
};

struct Roughness
{
    float roughness;
};

ConstantBuffer<Roughness> RoughnessCB : register(b0);

ConstantBuffer<DirectionalLight> DirectionalLightCB : register(b2);
ConstantBuffer<PointLight> PointLightCB : register(b3);


float3 CalculateDirectionalLight(const float3 viewPos, const float3 realViewNormal, const float3 specularColor)
{
    const float attenuation = 1.0f;
    const float3 diffuse = Diffuse(DirectionalLightCB.diffuseColor, DirectionalLightCB.diffuseIntensity, attenuation, -DirectionalLightCB.lightDirection, realViewNormal);
	
    const float3 specular = Speculate(specularColor, DirectionalLightCB.diffuseIntensity * DirectionalLightCB.specularIntensity, realViewNormal,
                                    -DirectionalLightCB.lightDirection, viewPos, attenuation, RoughnessCB.roughness);
    
    return saturate(diffuse + DirectionalLightCB.ambient + specular);
}

float3 CalculatePointLight(const float3 viewPos, const float3 realViewNormal, const float3 specularColor)
{
    const LightVectorData lightVector = CalculateLightVectorData(PointLightCB.lightViewPos, viewPos);
    
    const float attenuation = Attenuate(PointLightCB.attenuationConst, PointLightCB.attenuationLin, PointLightCB.attenuationQuad, lightVector.distanceToLight);
    const float3 diffuse = Diffuse(PointLightCB.diffuseColor, PointLightCB.diffuseIntensity, attenuation, lightVector.directionToLight, realViewNormal);
	
    const float3 specular = Speculate(specularColor, PointLightCB.diffuseIntensity * PointLightCB.specularIntensity, realViewNormal,
                                    lightVector.vectorToLight, viewPos, attenuation, RoughnessCB.roughness);
    
    return saturate(diffuse + PointLightCB.ambient + specular);
}

float3 CalculateFinalAmountOfLight(const float3 viewPos, const float3 realViewNormal, const float4 lightPerspectivePos, const float3 specularColor)
{
    const float lighting = 1.0f;
    //const float lighting = CalculateLighting(lightPerspectivePos);

    const float3 directionalLight = CalculateDirectionalLight(viewPos, realViewNormal, specularColor);
    const float3 pointLight = CalculatePointLight(viewPos, realViewNormal, specularColor);
    
    const float3 light = lighting * saturate(directionalLight + pointLight);
    const float3 shadow = (1.0f - lighting) * DirectionalLightCB.ambient;
    
    return light + shadow;
}


