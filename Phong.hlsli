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

TextureCube shadowMap : register(t0);
SamplerComparisonState shadowSampler : register(s0);

static const float zf = 200.0f;
static const float zn = 0.5f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

float CalculateLighting(float4 lightPerspectivePos)
{
    // get magnitudes for each basis component
    const float3 m = abs(lightPerspectivePos).xyz;
    // get the length in the dominant axis
    // (this correlates with shadow map face and derives comparison depth)
    const float major = max(m.x, max(m.y, m.z));
    // converting from distance in shadow light space to projected depth
    return shadowMap.SampleCmpLevelZero(shadowSampler, lightPerspectivePos.xyz, (c1 * major + c0) / major);
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
    
    const float lighting = CalculateLighting(lightPerspectivePos);
 
    const float attenuation = Attenuate(attenuationConst, attenuationLin, attenuationQuad, lightVector.distanceToLight);
	
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, attenuation, lightVector.directionToLight, realViewNormal);
	
    const float3 specular = Speculate(specularColor, diffuseIntensity * specularIntensity, realViewNormal, lightVector.vectorToLight, viewPos, attenuation, roughness);
	
    const float3 light = lighting * saturate(diffuse + ambient + specular);
    const float3 shadow = (1.0f - lighting) * ambient;
    
    return light + shadow;
}


