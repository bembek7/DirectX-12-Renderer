#include "ComparisonSampler.hlsli"
#include "LightDepthBufferCube.hlsli"

// rember that Z is reversed
static const float zf = 0.5f;
static const float zn = 100.0f;
static const float c1 = zf / (zf - zn);
static const float c0 = -zn * zf / (zf - zn);

float CalculateLightingCube(float4 lightPerspectivePos)
{
    // get magnitudes for each basis component
    const float3 m = abs(lightPerspectivePos).xyz;
    // get the length in the dominant axis
    // (this correlates with shadow map face and derives comparison depth)
    const float major = max(m.x, max(m.y, m.z));
    // converting from distance in shadow light space to projected depth
    const float depth = (c1 * major + c0) / major;
    return lightDepthBufferCube.SampleCmpLevelZero(comparisonSampler, lightPerspectivePos.xyz, depth);
}