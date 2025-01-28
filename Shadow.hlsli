#include "ComparisonSampler.hlsli"
#include "LightDepthBuffer.hlsli"

//static const float zf = 200.0f;
//static const float zn = 0.5f;
//static const float c1 = zf / (zf - zn);
//static const float c0 = -zn * zf / (zf - zn);

//float CalculateLighting(float4 lightPerspectivePos)
//{
//    // get magnitudes for each basis component
//    const float3 m = abs(lightPerspectivePos).xyz;
//    // get the length in the dominant axis
//    // (this correlates with shadow map face and derives comparison depth)
//    const float major = max(m.x, max(m.y, m.z));
//    // converting from distance in shadow light space to projected depth
//    const float depth = (c1 * major + c0) / major;
//    return shadowMap.SampleCmpLevelZero(shadowSampler, lightPerspectivePos.xy, depth);
//}

float CalculateLighting(float4 lightPerspectivePos)
{
    float2 texCoords;
    texCoords.x = 0.5f + (lightPerspectivePos.x / lightPerspectivePos.w * 0.5f);
    texCoords.y = 0.5f - (lightPerspectivePos.y / lightPerspectivePos.w * 0.5f);
    if (texCoords.x < 0.0f || texCoords.x > 1.0f || texCoords.y < 0.0f || texCoords.y > 1.0f)
    {
        return 0.f;
    }
    
    float pixelDepth = lightPerspectivePos.z / lightPerspectivePos.w;
    return lightDepthBuffer.SampleCmpLevelZero(comparisonSampler, texCoords, pixelDepth).r;
}
