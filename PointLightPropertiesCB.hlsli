struct PointLightProperties
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

ConstantBuffer<PointLightProperties> PointLightPropertiesCB : register(b0);