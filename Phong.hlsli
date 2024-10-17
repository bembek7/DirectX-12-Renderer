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

//struct Light
//{
//    float3 diffuseColor;        //all
//    float diffuseIntensity;     //all
//    float3 ambient;             //all
//    float specularIntensity;    //all
//    float3 lightViewPos;        //point/spot
//    float attenuationConst;     //point/spot
//    float attenuationLin;       //point/spot
//    float attenuationQuad;      //point/spot
//};

struct Light
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

ConstantBuffer<Light> LightCB : register(b0);

ConstantBuffer<Roughness> RoughnessCB : register(b1);

float3 CalculateFinalAmountOfLight(const float3 viewPos, const float3 realViewNormal, const float4 lightPerspectivePos, const float3 specularColor)
{
    //LightVectorData lightVector = CalculateLightVectorData(LightCB.lightViewPos, viewPos);
    
    const float lighting = 1.0f;
    //const float lighting = CalculateLighting(lightPerspectivePos);

    //const float attenuation = Attenuate(LightCB.attenuationConst, LightCB.attenuationLin, LightCB.attenuationQuad, lightVector.distanceToLight);
    const float attenuation = 1.0f;
    const float3 diffuse = Diffuse(LightCB.diffuseColor, LightCB.diffuseIntensity, attenuation, LightCB.lightDirection, realViewNormal);
	
    const float3 specular = Speculate(specularColor, LightCB.diffuseIntensity * LightCB.specularIntensity, realViewNormal, LightCB.lightDirection, viewPos, attenuation, RoughnessCB.roughness);
	
    const float3 light = lighting * saturate(diffuse + LightCB.ambient + specular);
    const float3 shadow = (1.0f - lighting) * LightCB.ambient;
    
    return light + shadow;
}


