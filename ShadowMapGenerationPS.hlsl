Texture2D lightPerspectiveZ : register(t0);
SamplerComparisonState shadowSampler : register(s0);

float main(float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POS) : SV_TARGET
{
    float2 shadowTexCoords;
    shadowTexCoords.x = 0.5f + (lightPerspectivePos.x / lightPerspectivePos.w * 0.5f);
    shadowTexCoords.y = 0.5f - (lightPerspectivePos.y / lightPerspectivePos.w * 0.5f);
    float pixelDepth = lightPerspectivePos.z / lightPerspectivePos.w;
    float depth = lightPerspectivePos.z;
    
    return lightPerspectiveZ.SampleCmpLevelZero(shadowSampler, shadowTexCoords, pixelDepth).r;
}