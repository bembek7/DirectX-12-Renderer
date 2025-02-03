struct DirectionalLight
{
    float3 diffuseColor;
    float diffuseIntensity;     
    float3 ambient;             
    float specularIntensity;    
    float3 lightDirection;      
};

ConstantBuffer<DirectionalLight> DirectionalLightPropertiesCB : register(b0);