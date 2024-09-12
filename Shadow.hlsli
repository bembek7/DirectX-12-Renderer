
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
