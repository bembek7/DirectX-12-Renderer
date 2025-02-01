#include "ComparisonSampler.hlsli"
#include "LightDepthBuffer.hlsli"

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
