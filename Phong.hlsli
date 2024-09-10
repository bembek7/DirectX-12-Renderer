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

#define PCF_RANGE 2

float CalculateLighting(float4 lightPerspectivePos, float3 directionToLight, float3 viewNormal)
{
    float2 shadowTexCoords;
    shadowTexCoords.x = 0.5f + (lightPerspectivePos.x / lightPerspectivePos.w * 0.5f);
    shadowTexCoords.y = 0.5f - (lightPerspectivePos.y / lightPerspectivePos.w * 0.5f);
    const float pixelDepth = lightPerspectivePos.z / lightPerspectivePos.w;
  
    float lighting = 1.f;
    
    // Check if the pixel texture coordinate is in the view frustum of the light before doing any shadow work.
    if (shadowTexCoords.x >= 0.f && shadowTexCoords.x <= 1.f &&
    shadowTexCoords.y >= 0.f && shadowTexCoords.y <= 1.f &&
    pixelDepth >= 0.f && pixelDepth <= 1.f)
    {
        lighting = 0.0f;
        [unroll]
        for (int x = -PCF_RANGE; x <= PCF_RANGE; x++)
        {
            [unroll]
            for (int y = -PCF_RANGE; y <= PCF_RANGE; y++)
            {
                //const float margin = acos(saturate(max(0.f, min(dot(directionToLight, viewNormal), 0.95f))));
                //float epsilon = 0.00005f / margin;
                //epsilon = clamp(epsilon, 0.f, 0.1f);
                lighting += shadowMap.SampleCmpLevelZero(shadowSampler, shadowTexCoords, pixelDepth, int2(x, y));
            }
        }
        lighting = lighting / ((PCF_RANGE * 2 + 1) * (PCF_RANGE * 2 + 1));
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

cbuffer LightCBuf : register(b0)
{
    const float3 diffuseColor;
    const float diffuseIntensity;
    const float3 ambient;
    const float specularIntensity;
    const float3 lightViewPos;
    const float attenuationConst;
    const float attenuationLin;
    const float attenuationQuad;
};

cbuffer RoughnessCBuf : register(b1)
{
    const float roughness;
};

float3 CalulateFinalAmountOfLight(const float3 viewPos, const float3 realViewNormal, const float4 lightPerspectivePos, const float3 specularColor)
{
    const LightVectorData lightVector = CalculateLightVectorData(lightViewPos, viewPos);
    
    const float lighting = CalculateLighting(lightPerspectivePos, lightVector.directionToLight, realViewNormal);
 
    const float attenuation = Attenuate(attenuationConst, attenuationLin, attenuationQuad, lightVector.distanceToLight);
	
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, attenuation, lightVector.directionToLight, realViewNormal);
	
    const float3 specular = Speculate(specularColor, diffuseIntensity * specularIntensity, realViewNormal, lightVector.vectorToLight, viewPos, attenuation, roughness);
	
    const float3 light = lighting * saturate(diffuse + ambient + specular);
    const float3 shadow = (1.0f - lighting) * ambient;
    
    return light + shadow;
}


