#include "MainDepthBuffer.hlsli"
#include "ComparisonSampler.hlsli"

float main(float4 pos : SV_Position) : SV_TARGET
{
    float2 pixelCoords;
    pixelCoords.x = 0.5f + (pos.x / pos.w * 0.5f);
    pixelCoords.y = 0.5f - (pos.y / pos.w * 0.5f);
    float pixelDepth = pos.z / pos.w;
    return mainDepthBuffer.SampleCmpLevelZero(comparisonSampler, pixelCoords, pixelDepth).r;
}