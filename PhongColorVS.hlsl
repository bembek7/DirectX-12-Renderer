struct Transform
{
    matrix model;
    matrix modelView;
    matrix modelViewProj;
};

struct ShadowMap
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

ConstantBuffer<Transform> TransformCB : register(b0);
ConstantBuffer<ShadowMap> ShadowMapCB : register(b1);

VSOut main(float3 position : POSITION, float3 normal : NORMAL)
{
    VSOut vsout;
    vsout.viewPos = (float3) mul(float4(position, 1.0f), TransformCB.modelView);
    vsout.viewNormal = mul(normal, (float3x3) TransformCB.modelView);
    vsout.pos = mul(float4(position, 1.0f), TransformCB.modelViewProj);
    
    // Transform the vertex position into projected space from the POV of the light.
    const float4 modelPos = mul(float4(position, 1.0f), TransformCB.model);
    vsout.lightPerspectivePos = mul(modelPos, ShadowMapCB.lightPerspective);
    
    return vsout;
}