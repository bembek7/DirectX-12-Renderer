struct SpotLightProperties
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

ConstantBuffer<SpotLightProperties> SpotLightPropertiesCB : register(b0);