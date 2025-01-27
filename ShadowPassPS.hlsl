#include "MainDepthBuffer.hlsli"
#include "ComparisonSampler.hlsli"

float main(float4 mainPerspectivePos : MAIN_PERSPECTIVE_POSITION) : SV_TARGET
{
    float2 pixelCoords;
    pixelCoords.x = 0.5f + (mainPerspectivePos.x / mainPerspectivePos.w * 0.5f);
    pixelCoords.y = 0.5f - (mainPerspectivePos.y / mainPerspectivePos.w * 0.5f);
    float pixelDepth = mainPerspectivePos.z / mainPerspectivePos.w;
    return mainDepthBuffer.SampleCmpLevelZero(comparisonSampler, pixelCoords, pixelDepth).r;
}