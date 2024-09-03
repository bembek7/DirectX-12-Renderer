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

Texture2D shadowMap : register(t0);
SamplerComparisonState shadowSampler : register(s0);

float CalculateLighting(float4 lightPerspectivePos, float3 directionToLight, float3 viewNormal)
{
    return 1;
    float2 shadowTexCoords;
    shadowTexCoords.x = 0.5f + (lightPerspectivePos.x / lightPerspectivePos.w * 0.5f);
    shadowTexCoords.y = 0.5f - (lightPerspectivePos.y / lightPerspectivePos.w * 0.5f);
    float pixelDepth = lightPerspectivePos.z / lightPerspectivePos.w;
  
    float lighting = 1;
    
    // Check if the pixel texture coordinate is in the view frustum of the light before doing any shadow work.
    if (saturate(shadowTexCoords.x) == shadowTexCoords.x &&
    saturate(shadowTexCoords.y) == shadowTexCoords.y &&
    pixelDepth > 0)
    {
        float margin = acos(saturate(max(0.f, min(dot(directionToLight, viewNormal), 0.95f))));

        float epsilon = 0.0005f / margin;
        epsilon = clamp(epsilon, 0.f, 0.1f);
        
        lighting = shadowMap.SampleCmp(shadowSampler, shadowTexCoords, pixelDepth + epsilon).r;
    }
    
    return lighting;
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


