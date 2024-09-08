cbuffer TransformCBuf : register(b0)
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};

cbuffer ShadowMapCBuf : register(b1)
{
    matrix lightPerspective;
};

struct VSOut
{
    float3 viewPos : POSITION;
    float3 viewNormal : NORMAL;
    float4 lightPerspectivePos : LIGHT_PERSPECTIVE_POSITION;
    float4 pos : SV_Position;
};

VSOut main(float3 position : POSITION, float3 normal : NORMAL)
{
    VSOut vsout;
    vsout.viewPos = (float3) mul(float4(position, 1.0f), modelView);
    vsout.viewNormal = mul(normal, (float3x3) modelView);
    vsout.pos = mul(float4(position, 1.0f), modelViewProj);
    
    // Transform the vertex position into projected space from the POV of the light.
    const float4 modelPos = mul(float4(position, 1.0f), model);
    vsout.lightPerspectivePos = mul(modelPos, lightPerspective);
    
    return vsout;
}